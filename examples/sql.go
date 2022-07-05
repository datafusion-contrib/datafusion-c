package main

// #cgo CFLAGS: -g -Wall -I${SRCDIR}/../include
// #cgo LDFLAGS: -L${SRCDIR}/../target/debug -ldatafusion
// #include <stdlib.h>
// #include <dlfcn.h>
// #include <datafusion.h>
import "C"
import (
	"fmt"
	"unsafe"
)

func main() {
	datafusionlib := C.df_session_context_new()
	if datafusionlib != nil {
		s := C.CString("SELECT 100;")
		defer C.free(unsafe.Pointer(s))

		var perror unsafe.Pointer

		data_frame := C.df_session_context_sql(
			datafusionlib, s, (**C.struct_DFError_)(unsafe.Pointer(&perror)))

		defer C.free(unsafe.Pointer(data_frame))

		x := (*[2]uintptr)(unsafe.Pointer(&perror))
		if x[0] != 0 {
			message := C.df_error_get_message((*C.struct_DFError_)(perror))
			fmt.Println(C.GoString(message))
			C.df_error_free((*C.struct_DFError_)(perror))
			return
		}

		C.df_data_frame_show(data_frame, (**C.struct_DFError_)(unsafe.Pointer(&perror)))
		y := (*[2]uintptr)(unsafe.Pointer(&perror))
		if y[0] != 0 {
			message := C.df_error_get_message((*C.struct_DFError_)(perror))
			fmt.Println(C.GoString(message))
			C.df_error_free((*C.struct_DFError_)(perror))
			return
		}

	}
	/*
		try:
		    error = (ctypes.c_void_p * 1)()
		    try:
		        data_frame = datafusion.df_session_context_sql(
		            context, b'SELECT 1;', ctypes.pointer(error))
		        if error[0] is not None:
		            message = datafusion.df_error_get_message(error[0])
		            print(f'failed to run SQL: {message.decode()}')
		            exit(1)
		        try:
		            datafusion.df_data_frame_show(data_frame, ctypes.pointer(error));
		            if error[0] is not None:
		                message = datafusion.df_error_get_message(error[0])
		                print('failed to show data frame: {message.decode()}')
		                exit(1)
		        finally:
		            datafusion.df_data_frame_free(data_frame)
		    finally:
		        if error[0] is not None:
		            datafusion.df_error_free(error[0])
		finally:
		    datafusion.df_session_context_free(context)
	*/
	//defer C.free(unsafe.Pointer(x))
	/*
		//doit()
		//name := C
		libpath := C.CString("libdatafusion.so")

		defer C.free(unsafe.Pointer(libpath))

		datafusionlib := C.dlopen(libpath, C.RTLD_LAZY)

		//defer C.free(unsafe.Pointer(p))
		//var imghandle int

		if datafusionlib != nil {
			//openimage := C.CString("openimage")
			//defer C.free(unsafe.Pointer(openimage))
			//fp := C.dlsym(imglib, openimage)
			//if fp != nil {
			//   fi := C.CString("fake.img")
			//   defer C.free(unsafe.Pointer(fi))
			//   imghandle = int(C.bridge_someFunc(C.someFunc(fp), fi))

			//} else {
			//    imghandle = myOpenImage("fake.img")
			//}
			//	C.dlclose(datafusionlib)
		}
		fmt.Println("Done")
	*/
}
