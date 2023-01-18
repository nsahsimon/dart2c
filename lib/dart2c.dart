import 'dart:async';
import 'package:flutter/services.dart';
import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'dart:io';
import 'dart:isolate';
import 'package:flutter/material.dart';
import 'package:flutter/foundation.dart';
import 'dart:core';
import 'package:image_picker/image_picker.dart';
import 'package:path_provider/path_provider.dart';
import 'dart:typed_data';


/* Add the dart implementation of native C/C++ functions here */


Future<int> _nativeAdd(Map data) async{
  ///unpacking data
  int num1 = data["num1"];
  int num2 = data["num2"];

  final DynamicLibrary nativeAddLib = Platform.isAndroid
      ? DynamicLibrary.open("libnative_add.so")
      : DynamicLibrary.process();

  int Function(int, int) nativeAddDart = nativeAddLib.lookup<NativeFunction<Int Function(Int, Int)>>("native_add").asFunction();

  int result = nativeAddDart(num1,num2);

  return result;
}

// Future<List<Map<String, dynamic>?>> _gradeMCQ(Map data, DynamicLibrary omrLib) async{
//   ///unpacking data
//   List correctAnswers = data["correct_answers"];
//   List<String> paths = data["paths"];
//   int subType = data['sub_type'];
//
//
//   Pointer<Utf8> Function(Pointer<Utf8> path, Pointer<Uint8> correctAnswers, Pointer<Uint8> responsesPtr, Pointer<Uint8> studentIdPtr, Pointer<Uint8> classIdPtr, Pointer<Uint8> subjectIdPtr, int subType ) gradeMCQDart = omrLib.lookup<NativeFunction<Pointer<Utf8> Function(Pointer<Utf8>, Pointer<Uint8>, Pointer<Uint8>, Pointer<Uint8>, Pointer<Uint8>, Pointer<Uint8>, Int32)>>("gradeMCQ").asFunction();
//   Pointer<Uint8> correctAnswersPtr = correctAnswers.join("").toNativeUtf8().cast<Uint8>();
//   Pointer<Uint8> responsesPtr = malloc.allocate(sizeOf<Uint8>() * (correctAnswers.length + 1));
//   Pointer<Uint8> classIdPtr = malloc.allocate(sizeOf<Uint8>() * 6); //5 bytes for the classId and 1 byte for the null termination
//   Pointer<Uint8> studentIdPtr = malloc.allocate(sizeOf<Uint8>() * 6);
//   Pointer<Uint8> subjectIdPtr = malloc.allocate(sizeOf<Uint8>() * 6);
//
//   List<Map<String, dynamic>?> results = [];
//
//   for(String path in paths) {
//     Map<String, dynamic>? result = {};
//     Pointer<Utf8> pathPtr = path.toNativeUtf8();
//     Pointer<Utf8> exitCodePtr =  gradeMCQDart(pathPtr, correctAnswersPtr, responsesPtr, studentIdPtr, classIdPtr, subjectIdPtr, subType);
//
//     try {
//       int exitCode = int.parse(exitCodePtr.cast<Utf8>().toDartString());
//       String responsesStr = responsesPtr.cast<Utf8>().toDartString();
//       String classIdStr = classIdPtr.cast<Utf8>().toDartString();
//       String studentIdStr = studentIdPtr.cast<Utf8>().toDartString();
//       String subjectIdStr = subjectIdPtr.cast<Utf8>().toDartString();
//       if(exitCode == 0) {
//         result = {
//           "student_id" : studentIdStr,
//           "student_name" : "N/A",
//           "class_id" : classIdStr,
//           "subject_id" : subjectIdStr,
//           "responses" : responsesStr.split(""),
//         };
//         debugPrint("Successfully graded script");
//         debugPrint("student_id: $studentIdStr");
//       }else {
//         debugPrint("Something went wrong, Exit Code: $exitCode");
//         result = null;
//       }
//     }catch (e) {
//       // TODO
//       debugPrint("$e");
//       debugPrint("Unable to extract results from pointers");
//     }
//     results.add(result);
//   }
//
//   ///free memory
//   malloc.free(responsesPtr);
//   malloc.free(classIdPtr);
//   malloc.free(studentIdPtr);
//   malloc.free(subjectIdPtr);
//   return results;
// }
//
// Future<List<Map<String, dynamic>?>> _record(Map data, DynamicLibrary omrLib) async{
//   ///unpacking data
//   List<String> paths = data["paths"];
//   List<String> savePathDirs = data["save_path_dirs"];
//   String root = data["root"];
//
//   Pointer<Utf8> Function(Pointer<Utf8> path, Pointer<Utf8> savePathDir, Pointer<Uint8> studentIdPtr, Pointer<Uint8> classIdPtr, Pointer<Uint8> subjectIdPtr) recordDart = omrLib.lookup<NativeFunction<Pointer<Utf8> Function(Pointer<Utf8>, Pointer<Utf8>, Pointer<Uint8>, Pointer<Uint8>, Pointer<Uint8>)>>("record").asFunction();
//   Pointer<Uint8> classIdPtr = malloc.allocate(sizeOf<Uint8>() * 6); //5 bytes for the classId and 1 byte for the null termination
//   Pointer<Uint8> studentIdPtr = malloc.allocate(sizeOf<Uint8>() * 6);
//   Pointer<Uint8> subjectIdPtr = malloc.allocate(sizeOf<Uint8>() * 6);
//
//   List<Map<String, dynamic>?> results = [];
//
//   int index = 0;
//   for(String path in paths) {
//     /// Create temporal directories to hold the images for the neural networks
//     String nameCharsPath = "$root/.temp/$index/name_chars"; // Defining Path to Directory where the name characters will be dumped
//     String scoreDigitsPath = "$root/.temp/$index/score_digits"; // Defining Path to the Directory where the score digits will be stored
//     createDirs([nameCharsPath, scoreDigitsPath]); // Explicitly Creating the directories for dumping the name_char and score_digit preprocessed images
//
//     Map<String, dynamic>? result;
//
//     Pointer<Utf8> pathPtr = path.toNativeUtf8();
//     Pointer<Utf8> savePathDirPtr = savePathDirs[index].toNativeUtf8();
//     debugPrint(">> Running the C++ program");
//     Pointer<Utf8> exitCodePtr =  recordDart(pathPtr, savePathDirPtr, studentIdPtr, classIdPtr, subjectIdPtr);
//     debugPrint(">> C++ program successfully run");
//
//     try {
//       int exitCode = int.parse(exitCodePtr.cast<Utf8>().toDartString());
//       String classIdStr = classIdPtr.cast<Utf8>().toDartString();
//       String studentIdStr = studentIdPtr.cast<Utf8>().toDartString();
//       String subjectIdStr = subjectIdPtr.cast<Utf8>().toDartString();
//       if(exitCode == 0) {
//         result = {
//           "student_id" : studentIdStr,
//           "student_name" : "N/A",
//           "class_id" : classIdStr,
//           "subject_id" : subjectIdStr,
//           "score" : ""
//         };
//       }else {
//         debugPrint("Something went wrong, Exit Code: $exitCode");
//         result = null;
//       }
//     } catch (e) {
//       // TODO
//       debugPrint("$e");
//       debugPrint("Unable to extract results from pointers");
//     }
//     results.add(result);
//     index++;
//   }
//   return results;
// }

// void createDirs(List<String> dirPaths){
//   for(String dir in dirPaths) {
//     debugPrint(">> Creating the directory for name characters");
//     ///for creating the director for name characters
//     try {
//       if(!Directory(dir).existsSync()) {
//         Directory(dir).createSync(recursive: true);
//         debugPrint(">> Path to name chars has just been created");
//       } else { //if the directory already exists
//         debugPrint(">> Path to name chars already exists");
//         Directory(dir).deleteSync(recursive: true);
//         Directory(dir).createSync(recursive: true);
//       }
//     } catch (e) {
//       debugPrint("$e");
//       debugPrint(">> Could not create image directories for neural nets");
//     }
//   }
// }

///The methods of this class will be called in the native isolate
class NativeIsolate {
  static late final DynamicLibrary nativeAddLib;
  static late final ReceivePort nativeIsolateReceivePort;


  static void nativeIsolate(SendPort sendPort) {
    /// Load the native dynamic libraries
    try {
      debugPrint(">>(Native Isolate) Trying to load Native Library");
      nativeAddLib = Platform.isAndroid
          ? DynamicLibrary.open("libnative_add.so")
          : DynamicLibrary.process();
      debugPrint(">>(Native Isolate) SUCCESSFULLY loaded the Native dynamic library");
    }catch (e) {
      debugPrint("$e");
      debugPrint(">>(Native Isolate) FAILED to load Native dynamic library");
    }

    /// Create a receiver port for this isolate
    nativeIsolateReceivePort = ReceivePort();

    ///Send the corresponding send port back to the main isolate
    sendPort.send(nativeIsolateReceivePort.sendPort);

    nativeIsolateReceivePort.listen(
            (message) async{
              if(message is Map<String, dynamic>) { //i.e if
                if(message['process'] == 'native_add'){
                  int result = await _nativeAdd(message);
                  sendPort.send(result);
                }else {

                }
              }
            });
  }
}

///The methods of this class will be called from the main isolate
class Native {
  static late final ReceivePort mainReceivePort;
  static late final SendPort mainSendPort;
  static late final Isolate? nativeIsolate;
  static StreamController? resultStreamController;

  Future<void> initialize() async {
    mainReceivePort = ReceivePort();

    /// Create native isolate
    nativeIsolate = await Isolate.spawn(NativeIsolate.nativeIsolate , mainReceivePort.sendPort);
    /// Start Listing to the stream
    mainReceivePort.listen(
            (message) {
              debugPrint("Received a message from the native isolate. Content: ${message}");
                if(message is SendPort) {
                  mainSendPort = message;
                  debugPrint("(Native initialize) SUCCESSFULLY retrieved send port from native isolate");
                } else {
                  debugPrint("(Native initialize) SUCCESSFULLY retrieved a result from the native isolate");
                  if(resultStreamController != null) {
                    resultStreamController!.add(message);
                  }
                }
    });

    debugPrint(">> Initializing the native library");
    debugPrint(">> Loading the dynamic libraries");
  }

  Future<int> nativeAdd({required int num1, required int num2}) async{
    // return await _processImage({"path": path, "correct_answers": correctAnswers});
    resultStreamController = StreamController();
    mainSendPort.send({"num1":  num1, "num2": num2, "process": "native_add"});
    int result = await resultStreamController!.stream.first;
    return result;
  }

}





