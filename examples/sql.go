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

}
