// Generated by jextract

package org.apache.arrow.datafusion;

import java.lang.invoke.MethodHandle;
import java.lang.invoke.VarHandle;
import java.nio.ByteOrder;
import java.lang.foreign.*;
import static java.lang.foreign.ValueLayout.*;
public class datafusion_h  {

    /* package-private */ datafusion_h() {}
    public static OfByte C_CHAR = Constants$root.C_CHAR$LAYOUT;
    public static OfShort C_SHORT = Constants$root.C_SHORT$LAYOUT;
    public static OfInt C_INT = Constants$root.C_INT$LAYOUT;
    public static OfLong C_LONG = Constants$root.C_LONG_LONG$LAYOUT;
    public static OfLong C_LONG_LONG = Constants$root.C_LONG_LONG$LAYOUT;
    public static OfFloat C_FLOAT = Constants$root.C_FLOAT$LAYOUT;
    public static OfDouble C_DOUBLE = Constants$root.C_DOUBLE$LAYOUT;
    public static OfAddress C_POINTER = Constants$root.C_POINTER$LAYOUT;
    public static MethodHandle df_error_free$MH() {
        return RuntimeHelper.requireNonNull(constants$0.df_error_free$MH,"df_error_free");
    }
    public static void df_error_free ( Addressable error) {
        var mh$ = df_error_free$MH();
        try {
            mh$.invokeExact(error);
        } catch (Throwable ex$) {
            throw new AssertionError("should not reach here", ex$);
        }
    }
    public static MethodHandle df_error_get_message$MH() {
        return RuntimeHelper.requireNonNull(constants$0.df_error_get_message$MH,"df_error_get_message");
    }
    public static MemoryAddress df_error_get_message ( Addressable error) {
        var mh$ = df_error_get_message$MH();
        try {
            return (java.lang.foreign.MemoryAddress)mh$.invokeExact(error);
        } catch (Throwable ex$) {
            throw new AssertionError("should not reach here", ex$);
        }
    }
    public static MethodHandle df_data_frame_free$MH() {
        return RuntimeHelper.requireNonNull(constants$0.df_data_frame_free$MH,"df_data_frame_free");
    }
    public static void df_data_frame_free ( Addressable data_frame) {
        var mh$ = df_data_frame_free$MH();
        try {
            mh$.invokeExact(data_frame);
        } catch (Throwable ex$) {
            throw new AssertionError("should not reach here", ex$);
        }
    }
    public static MethodHandle df_data_frame_show$MH() {
        return RuntimeHelper.requireNonNull(constants$0.df_data_frame_show$MH,"df_data_frame_show");
    }
    public static void df_data_frame_show ( Addressable data_frame,  Addressable error) {
        var mh$ = df_data_frame_show$MH();
        try {
            mh$.invokeExact(data_frame, error);
        } catch (Throwable ex$) {
            throw new AssertionError("should not reach here", ex$);
        }
    }
    public static MethodHandle df_session_context_new$MH() {
        return RuntimeHelper.requireNonNull(constants$0.df_session_context_new$MH,"df_session_context_new");
    }
    public static MemoryAddress df_session_context_new () {
        var mh$ = df_session_context_new$MH();
        try {
            return (java.lang.foreign.MemoryAddress)mh$.invokeExact();
        } catch (Throwable ex$) {
            throw new AssertionError("should not reach here", ex$);
        }
    }
    public static MethodHandle df_session_context_free$MH() {
        return RuntimeHelper.requireNonNull(constants$0.df_session_context_free$MH,"df_session_context_free");
    }
    public static void df_session_context_free ( Addressable ctx) {
        var mh$ = df_session_context_free$MH();
        try {
            mh$.invokeExact(ctx);
        } catch (Throwable ex$) {
            throw new AssertionError("should not reach here", ex$);
        }
    }
    public static MethodHandle df_session_context_sql$MH() {
        return RuntimeHelper.requireNonNull(constants$1.df_session_context_sql$MH,"df_session_context_sql");
    }
    public static MemoryAddress df_session_context_sql ( Addressable ctx,  Addressable sql,  Addressable error) {
        var mh$ = df_session_context_sql$MH();
        try {
            return (java.lang.foreign.MemoryAddress)mh$.invokeExact(ctx, sql, error);
        } catch (Throwable ex$) {
            throw new AssertionError("should not reach here", ex$);
        }
    }
}


