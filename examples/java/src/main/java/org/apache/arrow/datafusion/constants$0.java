// Generated by jextract

package org.apache.arrow.datafusion;

import java.lang.invoke.MethodHandle;
import java.lang.invoke.VarHandle;
import java.nio.ByteOrder;
import java.lang.foreign.*;
import static java.lang.foreign.ValueLayout.*;
class constants$0 {

    static final FunctionDescriptor df_error_free$FUNC = FunctionDescriptor.ofVoid(
        Constants$root.C_POINTER$LAYOUT
    );
    static final MethodHandle df_error_free$MH = RuntimeHelper.downcallHandle(
        "df_error_free",
        constants$0.df_error_free$FUNC
    );
    static final FunctionDescriptor df_error_get_message$FUNC = FunctionDescriptor.of(Constants$root.C_POINTER$LAYOUT,
        Constants$root.C_POINTER$LAYOUT
    );
    static final MethodHandle df_error_get_message$MH = RuntimeHelper.downcallHandle(
        "df_error_get_message",
        constants$0.df_error_get_message$FUNC
    );
    static final FunctionDescriptor df_data_frame_free$FUNC = FunctionDescriptor.ofVoid(
        Constants$root.C_POINTER$LAYOUT
    );
    static final MethodHandle df_data_frame_free$MH = RuntimeHelper.downcallHandle(
        "df_data_frame_free",
        constants$0.df_data_frame_free$FUNC
    );
    static final FunctionDescriptor df_data_frame_show$FUNC = FunctionDescriptor.ofVoid(
        Constants$root.C_POINTER$LAYOUT,
        Constants$root.C_POINTER$LAYOUT
    );
    static final MethodHandle df_data_frame_show$MH = RuntimeHelper.downcallHandle(
        "df_data_frame_show",
        constants$0.df_data_frame_show$FUNC
    );
    static final FunctionDescriptor df_session_context_new$FUNC = FunctionDescriptor.of(Constants$root.C_POINTER$LAYOUT);
    static final MethodHandle df_session_context_new$MH = RuntimeHelper.downcallHandle(
        "df_session_context_new",
        constants$0.df_session_context_new$FUNC
    );
    static final FunctionDescriptor df_session_context_free$FUNC = FunctionDescriptor.ofVoid(
        Constants$root.C_POINTER$LAYOUT
    );
    static final MethodHandle df_session_context_free$MH = RuntimeHelper.downcallHandle(
        "df_session_context_free",
        constants$0.df_session_context_free$FUNC
    );
}


