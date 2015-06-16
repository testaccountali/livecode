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

extern const uint1 MCisotranslations[256];
extern const uint1 MCmactranslations[256];

extern const char * const MCtoolnames[];

extern const uint4 MCbuildnumber;
extern const char * const MCversionstring;
extern const char * const MCcopystring;
extern const char * const MCstandardstring;
extern const char * const MCdialogstring;
extern const char * const MCmovablestring;
extern const char * const MCpalettestring;
extern const char * const MCsheetstring;
extern const char * const MCdrawerstring;
extern const char * const MCmodalstring;
extern const char * const MCmodelessstring;
extern const char * const MCtoplevelstring;
extern const char * const MCtransparentstring;
extern const char * const MCopaquestring;
extern const char * const MCrectanglestring;
extern const char * const MCshadowstring;
extern const char * const MCscrollingstring;
extern const char * const MCroundrectstring;
extern const char * const MCcheckboxstring;
extern const char * const MCradiobuttonstring;
extern const char * const MClinestring;
extern const char * const MCpolygonstring;
extern const char * const MCarcstring;
extern const char * const MCovalstring;
extern const char * const MCregularstring;
extern const char * const MCcurvestring;
extern const char * const MCtextstring;
extern const char * const MCleftstring;
extern const char * const MCcenterstring;
extern const char * const MCrightstring;
extern const char * const MCjustifystring;
extern const char * const MCplainstring;
extern const char * const MCmixedstring;
extern const char * const MCboxstring;
extern const char * const MCthreedboxstring;
extern const char * const MCunderlinestring;
extern const char * const MCstrikeoutstring;
extern const char * const MCgroupstring;
extern const char * const MClinkstring;
extern const char * const MCtruestring;
extern const char * const MCfalsestring;
extern const char * const MCdownstring;
extern const char * const MCupstring;
extern const char * const MCshiftstring;
extern const char * const MCcommandstring;
extern const char * const MCcontrolstring;
extern const char * const MCmod1string;
extern const char * const MCpulldownstring;
extern const char * const MCpopupstring;
extern const char * const MCoptionstring;
extern const char * const MCcascadestring;
extern const char * const MCcombostring;
extern const char * const MCtabstring;
extern const char * const MCstackstring;
extern const char * const MCaudiostring;
extern const char * const MCvideostring;
extern const char * const MCdefaultstring;
extern const char * const MCtitlestring;
extern const char * const MCmenustring;
extern const char * const MCminimizestring;
extern const char * const MCmaximizestring;
extern const char * const MCclosestring;
extern const char * const MCmetalstring;
extern const char * const MCutilitystring;
extern const char * const MCnoshadowstring;
extern const char * const MCforcetaskbarstring;
extern const char * const MCunicodestring;
extern const char * const MCnativestring;

extern const char * const MCbackgroundstring;
extern const char * const MCcardstring;
extern const char * const MCbuttonstring;
extern const char * const MCgraphicstring;
extern const char * const MCepsstring;
extern const char * const MCscrollbarstring;
extern const char * const MCplayerstring;
extern const char * const MCscalestring;
extern const char * const MCprogressstring;
extern const char * const MCimagestring;
extern const char * const MCfieldstring;
extern const char * const MCcolorstring;
extern const char * const MCmagnifierstring;

extern const char * const MCnotfoundstring;
extern const char * const MCplatformstring;
extern const char * const MClnfamstring;
extern const char * const MClnfmacstring;
extern const char * const MClnfmotifstring;
extern const char * const MClnfwinstring;
extern const char * const MCuntitledstring;
extern const char * const MCapplicationstring;
extern const char * const MCanswernamestring;
extern const char * const MCasknamestring;
extern const char * const MCfsnamestring;
extern const char * const MCcsnamestring;
extern const char * const MChelpnamestring;
extern const char * const MChomenamestring;
extern const char * const MChcstatnamestring;
extern const char * const MCmessagenamestring;
extern const char * const MCdonestring;
extern const char * const MCnullstring;
extern const char * const MCintersectstring;
extern const char * const MCsurroundstring;
extern const char * const MCtopstring;
extern const char * const MCbottomstring;
extern const char * const MCcancelstring;

extern const char * const MCliststylestrings[];
extern const char * const MCtextalignstrings[];

extern MCNameRef MCN_msg;
extern MCNameRef MCN_each;
extern MCNameRef MCN_it;

extern MCNameRef MCN_default_text_font;

extern MCNameRef MCM_apple_event;
extern MCNameRef MCM_arrow_key;
extern MCNameRef MCM_assert_error;
extern MCNameRef MCM_backspace_key;
extern MCNameRef MCM_close_background;
extern MCNameRef MCM_close_card;
extern MCNameRef MCM_close_control;
extern MCNameRef MCM_close_field;
extern MCNameRef MCM_close_stack;
extern MCNameRef MCM_close_stack_request;
extern MCNameRef MCM_color_changed;
extern MCNameRef MCM_command_key_down;
extern MCNameRef MCM_control_key_down;
extern MCNameRef MCM_copy_key;
extern MCNameRef MCM_current_time_changed;
extern MCNameRef MCM_cut_key;
extern MCNameRef MCM_debug_str;

extern MCNameRef MCM_delete_background;
extern MCNameRef MCM_delete_button;
extern MCNameRef MCM_delete_card;
extern MCNameRef MCM_delete_eps;
extern MCNameRef MCM_delete_field;
extern MCNameRef MCM_delete_graphic;
extern MCNameRef MCM_delete_group;
extern MCNameRef MCM_delete_image;
extern MCNameRef MCM_delete_scrollbar;
extern MCNameRef MCM_delete_player;
extern MCNameRef MCM_delete_stack;

extern MCNameRef MCM_delete_key;
extern MCNameRef MCM_delete_url;
extern MCNameRef MCM_desktop_changed;
extern MCNameRef MCM_drag_drop;
extern MCNameRef MCM_drag_end;
extern MCNameRef MCM_drag_enter;
extern MCNameRef MCM_drag_leave;
extern MCNameRef MCM_drag_move;
extern MCNameRef MCM_drag_start;
extern MCNameRef MCM_edit_script;
extern MCNameRef MCM_enter_in_field;
extern MCNameRef MCM_enter_key;
extern MCNameRef MCM_error_dialog;
extern MCNameRef MCM_escape_key;
extern MCNameRef MCM_eval;
extern MCNameRef MCM_exit_field;
extern MCNameRef MCM_focus_in;
extern MCNameRef MCM_focus_out;
extern MCNameRef MCM_function_key;
extern MCNameRef MCM_get_cached_urls;
extern MCNameRef MCM_get_url;
extern MCNameRef MCM_get_url_status;

// MM-2012-11-06: [[ Property Listener ]]
extern MCNameRef MCM_gradient_edit_ended;
extern MCNameRef MCM_gradient_edit_started;

extern MCNameRef MCM_help;
extern MCNameRef MCM_hot_spot_clicked;
extern MCNameRef MCM_icon_menu_pick;
extern MCNameRef MCM_icon_menu_opening;
extern MCNameRef MCM_status_icon_menu_pick;
extern MCNameRef MCM_status_icon_menu_opening;
extern MCNameRef MCM_status_icon_click;
extern MCNameRef MCM_status_icon_double_click;
extern MCNameRef MCM_iconify_stack;
extern MCNameRef MCM_id_changed;
extern MCNameRef MCM_idle;
extern MCNameRef MCM_license;
extern MCNameRef MCM_internal;
extern MCNameRef MCM_internal2;
extern MCNameRef MCM_internal3;
extern MCNameRef MCM_key_down;
extern MCNameRef MCM_key_up;
extern MCNameRef MCM_keyboard_activated;
extern MCNameRef MCM_keyboard_deactivated;
extern MCNameRef MCM_library_stack;
extern MCNameRef MCM_link_clicked;
extern MCNameRef MCM_load_url;
extern MCNameRef MCM_main_stack_changed;

// MW-2013-03-20: [[ MainStacksChanged ]]
extern MCNameRef MCM_main_stacks_changed;

extern MCNameRef MCM_menu_pick;
extern MCNameRef MCM_message;
extern MCNameRef MCM_message_handled;
extern MCNameRef MCM_message_not_handled;
extern MCNameRef MCM_mouse_double_down;
extern MCNameRef MCM_mouse_double_up;
extern MCNameRef MCM_mouse_down;
extern MCNameRef MCM_mouse_down_in_backdrop;
extern MCNameRef MCM_mouse_enter;
extern MCNameRef MCM_mouse_leave;
extern MCNameRef MCM_mouse_move;
extern MCNameRef MCM_mouse_release;
extern MCNameRef MCM_mouse_still_down;
extern MCNameRef MCM_mouse_up;
extern MCNameRef MCM_mouse_up_in_backdrop;
extern MCNameRef MCM_mouse_within;
extern MCNameRef MCM_move_control;
extern MCNameRef MCM_move_stack;
extern MCNameRef MCM_move_stopped;
extern MCNameRef MCM_movie_touched;
extern MCNameRef MCM_name_changed;
extern MCNameRef MCM_new_background;
extern MCNameRef MCM_new_card;
extern MCNameRef MCM_new_stack;
extern MCNameRef MCM_new_tool;
extern MCNameRef MCM_node_changed;

// MM-2012-11-05: [[ Object selection started/ended message ]]
extern MCNameRef MCM_object_selection_ended;
extern MCNameRef MCM_object_selection_started;

extern MCNameRef MCM_open_background;
extern MCNameRef MCM_open_control;
extern MCNameRef MCM_open_card;
extern MCNameRef MCM_open_field;
extern MCNameRef MCM_open_stack;
extern MCNameRef MCM_option_key_down;
extern MCNameRef MCM_paste_key;
extern MCNameRef MCM_play_paused;
extern MCNameRef MCM_play_rate_changed;
extern MCNameRef MCM_play_started;
extern MCNameRef MCM_play_stopped;
extern MCNameRef MCM_post_url;
extern MCNameRef MCM_preopen_background;
extern MCNameRef MCM_preopen_card;
extern MCNameRef MCM_preopen_control;
extern MCNameRef MCM_preopen_stack;

// MM-2012-09-05: [[ Property Listener ]]
extern MCNameRef MCM_property_changed;

extern MCNameRef MCM_put_url;
extern MCNameRef MCM_qtdebugstr;
extern MCNameRef MCM_raw_key_down;
extern MCNameRef MCM_raw_key_up;
#ifdef FEATURE_RELAUNCH_SUPPORT
extern MCNameRef MCM_relaunch;
#endif
extern MCNameRef MCM_release_stack;
extern MCNameRef MCM_reload_stack;
extern MCNameRef MCM_resize_control;

// MM-2012-11-06: [[ Property Listener ]]
extern MCNameRef MCM_resize_control_ended;
extern MCNameRef MCM_resize_control_started;

extern MCNameRef MCM_resize_stack;
extern MCNameRef MCM_resolution_error;
extern MCNameRef MCM_resume;
extern MCNameRef MCM_resume_stack;
extern MCNameRef MCM_return_in_field;
extern MCNameRef MCM_return_key;
extern MCNameRef MCM_save_stack_request;
extern MCNameRef MCM_script_error;
extern MCNameRef MCM_script_execution_error;
extern MCNameRef MCM_scrollbar_beginning;
extern MCNameRef MCM_scrollbar_drag;
extern MCNameRef MCM_scrollbar_end;
extern MCNameRef MCM_scrollbar_line_dec;
extern MCNameRef MCM_scrollbar_line_inc;
extern MCNameRef MCM_scrollbar_page_dec;
extern MCNameRef MCM_scrollbar_page_inc;
extern MCNameRef MCM_selected_object_changed;
extern MCNameRef MCM_selection_changed;
extern MCNameRef MCM_signal;
extern MCNameRef MCM_shut_down;
extern MCNameRef MCM_shut_down_request;
extern MCNameRef MCM_socket_error;
extern MCNameRef MCM_socket_closed;
extern MCNameRef MCM_socket_timeout;
extern MCNameRef MCM_start_up;
extern MCNameRef MCM_suspend;
extern MCNameRef MCM_suspend_stack;
extern MCNameRef MCM_tab_key;
extern MCNameRef MCM_text_changed;
extern MCNameRef MCM_trace;
extern MCNameRef MCM_trace_break;
extern MCNameRef MCM_trace_done;
extern MCNameRef MCM_trace_error;

extern MCNameRef MCM_undo_changed;
extern MCNameRef MCM_undo_key;
extern MCNameRef MCM_uniconify_stack;
extern MCNameRef MCM_unload_url;
extern MCNameRef MCM_update_var;

#ifdef _MOBILE
extern MCNameRef MCN_firstname;
extern MCNameRef MCN_lastname;
extern MCNameRef MCN_middlename;
extern MCNameRef MCN_prefix;
extern MCNameRef MCN_suffix;
extern MCNameRef MCN_nickname;
extern MCNameRef MCN_firstnamephonetic;
extern MCNameRef MCN_lastnamephonetic;
extern MCNameRef MCN_middlenamephonetic;
extern MCNameRef MCN_organization;
extern MCNameRef MCN_jobtitle;
extern MCNameRef MCN_department;
extern MCNameRef MCN_note;

extern MCNameRef MCN_email;
extern MCNameRef MCN_phone;
extern MCNameRef MCN_address;

extern MCNameRef MCN_home;
extern MCNameRef MCN_work;
extern MCNameRef MCN_other;
extern MCNameRef MCN_mobile;
extern MCNameRef MCN_iphone;
extern MCNameRef MCN_main;
extern MCNameRef MCN_homefax;
extern MCNameRef MCN_workfax;
extern MCNameRef MCN_otherfax;
extern MCNameRef MCN_pager;

extern MCNameRef MCN_street;
extern MCNameRef MCN_city;
extern MCNameRef MCN_state;
extern MCNameRef MCN_zip;
extern MCNameRef MCN_country;
extern MCNameRef MCN_countrycode;

extern MCNameRef MCM_touch_start;
extern MCNameRef MCM_touch_move;
extern MCNameRef MCM_touch_end;
extern MCNameRef MCM_touch_release;

extern MCNameRef MCM_motion_start;
extern MCNameRef MCM_motion_end;
extern MCNameRef MCM_motion_release;

extern MCNameRef MCM_url_progress;

extern MCNameRef MCM_acceleration_changed;

extern MCNameRef MCM_orientation_changed;

extern MCNameRef MCM_location_changed;
extern MCNameRef MCM_location_error;

extern MCNameRef MCM_heading_changed;
extern MCNameRef MCM_heading_error;
extern MCNameRef MCM_purchase_updated;

extern MCNameRef MCM_rotation_rate_changed;
extern MCNameRef MCM_tracking_error;

extern MCNameRef MCM_local_notification_received;
extern MCNameRef MCM_push_notification_received;
extern MCNameRef MCM_push_notification_registered;
extern MCNameRef MCM_push_notification_registration_error;
extern MCNameRef MCM_url_wake_up;

extern MCNameRef MCM_browser_started_loading;
extern MCNameRef MCM_browser_finished_loading;
extern MCNameRef MCM_browser_load_failed;

extern MCNameRef MCM_sound_finished_on_channel;

extern MCNameRef MCM_ad_loaded;
extern MCNameRef MCM_ad_clicked;
extern MCNameRef MCM_ad_load_failed;
extern MCNameRef MCM_ad_resize_start;
extern MCNameRef MCM_ad_resize_end;
extern MCNameRef MCM_ad_expand_start;
extern MCNameRef MCM_ad_expand_end;


extern MCNameRef MCM_scroller_did_scroll;
extern MCNameRef MCM_scroller_begin_drag;
extern MCNameRef MCM_scroller_end_drag;

extern MCNameRef MCM_player_finished;
extern MCNameRef MCM_player_error;
extern MCNameRef MCM_player_property_available;

extern MCNameRef MCM_input_begin_editing;
extern MCNameRef MCM_input_end_editing;
extern MCNameRef MCM_input_return_key;
extern MCNameRef MCM_input_text_changed;
extern MCNameRef MCM_product_details_received;
extern MCNameRef MCM_product_request_error;
#endif

#ifdef _IOS_MOBILE
extern MCNameRef MCM_browser_load_request;
extern MCNameRef MCM_browser_load_requested;
extern MCNameRef MCM_scroller_begin_decelerate;
extern MCNameRef MCM_scroller_end_decelerate;
extern MCNameRef MCM_scroller_scroll_to_top;
extern MCNameRef MCM_player_progress_changed;
extern MCNameRef MCM_player_enter_fullscreen;
extern MCNameRef MCM_player_leave_fullscreen;
extern MCNameRef MCM_player_state_changed;
extern MCNameRef MCM_player_movie_changed;
extern MCNameRef MCM_player_stopped;
extern MCNameRef MCM_reachability_changed;
//extern MCNameRef MCM_product_details_received;
//extern MCNameRef MCM_product_request_error;
extern MCNameRef MCM_protected_data_available;
extern MCNameRef MCM_protected_data_unavailable;

// MW-2013-05-30: [[ RemoteControl ]] Message sent when a remote control event is received.
extern MCNameRef MCM_remote_control_received;
#endif

