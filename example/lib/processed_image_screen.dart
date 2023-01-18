// import "package:flutter/material.dart";
// import "dart:io";
// import "package:modal_progress_hud_nsn/modal_progress_hud_nsn.dart";
// import "dart:ffi";
// import "package:omr/omr.dart";
// import "package:ffi/ffi.dart";
// import "package:auto_size_text/auto_size_text.dart";
//
// class ProcessedImageScreen extends StatefulWidget {
//   String imagePath;
//   ProcessedImageScreen(@required this.imagePath);
//
//   @override
//   _ProcessedImageScreenState createState() => _ProcessedImageScreenState();
// }
//
// class _ProcessedImageScreenState extends State<ProcessedImageScreen> {
//
//   bool isLoading = false;
//   late File imgFile;
//
//   void startLoading() {
//     setState((){
//       isLoading = true;
//     });
//   }
//
//   void stopLoading() {
//     setState((){
//       isLoading = true;
//     });
//   }
//
//
//   @override
//   void initState() {
//     super.initState();
//     Future(() {
//       compute();
//     });
//   }
//
//
//   void compute() async {
//     startLoading();
//     Pointer<Uint8> pathPtr = widget.imagePath.toNativeUtf8().cast<Uint8>();
//     try {
//       OmrScanner().extractForm(pathPtr);
//       setState((){
//         imgFile = File(widget.imagePath);
//       });
//     } catch(e) {
//       debugPrint('$e');
//       debugPrint("Execution of C++ code failed");
//       Navigator.pop(context);
//     }
//     stopLoading();
//   }
//
//
//
//
//
//   @override
//   Widget build(BuildContext context) {
//     return Scaffold(
//       appBar: AppBar(
//         title: const AutoSizeText("Processed Image", maxLines: 1),
//       ),
//       backgroundColor: Colors.white,
//       body: isLoading
//             ? ModalProgressHUD(inAsyncCall: isLoading,child: Container(child: null))
//              : Image.file(imgFile)
//     );
//   }
// }
