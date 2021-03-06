# Copyright (C) 2015 Runtime Revolution Ltd.
#
# This file is part of LiveCode.
#
# LiveCode is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3 as published by the Free
# Software Foundation.
#
# LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with LiveCode.  If not see <http://www.gnu.org/licenses/>.

# Usually, you'll just want to type "make all".

################################################################

# Tools that Make calls
XCODEBUILD ?= xcodebuild
WINE ?= wine
EMMAKE ?= emmake

# Some magic to control which versions of iOS we try to build.  N.b. you may
# also need to modify the buildbot configuration
IPHONEOS_VERSIONS ?= 8.2 8.4
IPHONESIMULATOR_VERSIONS ?= 6.1 7.1 8.2 8.4

IOS_SDKS ?= \
	$(addprefix iphoneos,$(IPHONEOS_VERSIONS)) \
	$(addprefix iphonesimulator,$(IPHONESIMULATOR_VERSIONS))

# Choose the correct build type
MODE ?= debug
ifeq ($(MODE),debug)
  export BUILDTYPE ?= Debug
else ifeq ($(MODE),release)
  export BUILDTYPE ?= Release
else ifeq ($(MODE),fast)
  export BUILDTYPE ?= Fast
else
  $(error "Mode must be 'debug' or 'release'")
endif

# Where to run the build command depends on community vs commercial
ifeq ($(BUILD_EDITION),commercial)
  BUILD_SUBDIR :=
  BUILD_PROJECT := livecode-commercial
else
  BUILD_SUBDIR := /livecode
  BUILD_PROJECT := livecode
endif

################################################################

.DEFAULT: all

guess_platform_script := \
	case `uname -s` in \
		Linux) echo linux ;; \
		Darwin) echo mac ;; \
	esac
guess_platform := $(shell $(guess_platform_script))

all: all-$(guess_platform)

################################################################
# Linux rules
################################################################

LINUX_ARCHS = x86_64 x86

guess_linux_arch_script := \
	case `uname -p` in \
		x86_64) echo x86_64 ;; \
		x86|i*86) echo x86 ;; \
	esac

guess_linux_arch := $(shell $(guess_linux_arch_script))

config-linux-%:
	./config.sh --platform linux-$*

compile-linux-%:
	$(MAKE) -C build-linux-$*/livecode

all-linux-%:
	$(MAKE) config-linux-$*
	$(MAKE) compile-linux-$*

$(addsuffix -linux,all config compile): %: %-$(guess_linux_arch)

################################################################
# Android rules
################################################################

ANDROID_ARCHS = armv6

config-android-%:
	./config.sh --platform android-$*

compile-android-%:
	$(MAKE) -C build-android-$*/livecode

all-android-%:
	$(MAKE) config-android-$*
	$(MAKE) compile-android-$*

$(addsuffix -android,all config compile): %: %-armv6

################################################################
# Mac rules
################################################################

config-mac:
	./config.sh --platform mac

compile-mac:
	$(XCODEBUILD) -project "build-mac$(BUILD_SUBDIR)/$(BUILD_PROJECT).xcodeproj" -configuration $(BUILDTYPE)

all-mac:
	$(MAKE) config-mac
	$(MAKE) compile-mac

################################################################
# iOS rules
################################################################

all-ios-%:
	$(MAKE) config-ios-$*
	$(MAKE) compile-ios-$*

config-ios-%:
	./config.sh --platform ios --generator-output build-ios-$*/livecode -Dtarget_sdk=$*

compile-ios-%:
	$(XCODEBUILD) -project "build-ios-$*$(BUILD_SUBDIR)/$(BUILD_PROJECT).xcodeproj" -configuration $(BUILDTYPE)

# Dummy targets to prevent our build system from building iOS 5.1 simulator
config-ios-iphonesimulator5.1:
	@echo "Skipping iOS simulator 5.1 (no longer supported)"
compile-ios-iphonesimulator5.1:
	@echo "Skipping iOS simulator 5.1 (no longer supported)"

# Provide some synonyms for "latest iOS SDK"
$(addsuffix -ios-iphoneos,all config compile): %: %8.4
	@true
$(addsuffix -ios-iphonesimulator,all config compile): %: %8.4
	@true

all_ios_subplatforms = iphoneos iphonesimulator $(IOS_SDKS)

all-ios: $(addprefix all-ios-,$(IOS_SDKS))
config-ios: $(addprefix config-ios-,$(IOS_SDKS))
compile-ios: $(addprefix compile-ios-,$(IOS_SDKS))

################################################################
# Windows rules
################################################################

config-win-%:
	./config.sh --platform win-$*

compile-win-%:
	# windows builds occur under Wine
	cd build-win-$* && $(WINE) /K ../make.cmd

all-win-%:
	$(MAKE) config-win-$*
	$(MAKE) compile-win-$*

$(addsuffix -win,all config compile): %: %-x86

################################################################
# Emscripten rules
################################################################

config-emscripten:
	$(EMMAKE) ./config.sh --platform emscripten

compile-emscripten:
	$(EMMAKE) $(MAKE) -C build-emscripten/livecode

all-emscripten:
	$(MAKE) config-emscripten
	$(MAKE) compile-emscripten
