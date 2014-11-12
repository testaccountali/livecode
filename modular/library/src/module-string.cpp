/* Copyright (C) 2003-2013 Runtime Revolution Ltd.
 
 This file is part of LiveCode.
 
 LiveCode is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License v3 as published by the Free
 Software Foundation.
 
 LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.
 
 You should have received a copy of the GNU General Public License
 along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

#include <foundation.h>
#include <foundation-auto.h>

void MCStringEvalConcatenate(MCStringRef p_left, MCStringRef p_right, MCStringRef& r_output)
{
    if (!MCStringFormat(r_output, "%@%@", p_left, p_right))
        return;
}

void MCStringExecPutStringBefore(MCStringRef p_source, MCStringRef& x_target)
{
    MCAutoStringRef t_string;
    MCStringEvalConcatenate(p_source, x_target, &t_string);
    
    MCValueAssign(x_target, *t_string);
}

void MCStringExecPutStringAfter(MCStringRef p_source, MCStringRef& x_target)
{
    MCAutoStringRef t_string;
    MCStringEvalConcatenate(x_target, p_source, &t_string);
    
    MCValueAssign(x_target, *t_string);
}

void MCStringEvalConcatenateWithSpace(MCStringRef p_left, MCStringRef p_right, MCStringRef& r_output)
{
    if (!MCStringFormat(r_output, "%@ %@", p_left, p_right))
        return;
}

void MCStringEvalLowercaseOf(MCStringRef p_source, MCStringRef& r_output)
{
    MCAutoStringRef t_string;
    if (!MCStringMutableCopy(p_source, &t_string))
        return;
    
    if (!MCStringLowercase(*t_string, kMCLocaleBasic))
        return;
    
    if (!MCStringCopy(*t_string, r_output))
        return;
}

void MCStringEvalUppercaseOf(MCStringRef p_source, MCStringRef& r_output)
{
    MCAutoStringRef t_string;
    if (!MCStringMutableCopy(p_source, &t_string))
        return;
    
    if (!MCStringUppercase(*t_string, kMCLocaleBasic))
        return;
    
    if (!MCStringCopy(*t_string, r_output))
        return;
}