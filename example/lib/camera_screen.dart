// import 'package:flutter/material.dart';
// import 'package:image_picker/image_picker.dart';
// import 'dart:io';
// import "package:omr_example/cmp_numbers.dart";
// import "processed_image_screen.dart";
//
//
// class CameraScreen extends StatefulWidget {
//   @override
//   _CameraScreenState createState() => _CameraScreenState();
// }
//
// class _CameraScreenState extends State<CameraScreen> {
//   File? imageFile;
//   late String imgPath;
//
//   _openGallery (BuildContext context)async{
//     var picture = await ImagePicker().getImage(source: ImageSource.gallery);
//     imgPath = picture!.path;
//     this.setState(() {
//       imageFile = File(picture.path);
//     });
//     Navigator.of(context).pop();
//   }
//
//   _openCamera(BuildContext context) async{
//     var picture = await ImagePicker().getImage(source: ImageSource.camera);
//     this.setState(() {
//       imageFile = File(picture!.path);
//     });
//     Navigator.of(context).pop();
//   }
//   Future<void> _showChoiceDialog (BuildContext context){
//     return showDialog(context: context,builder: (BuildContext context){
//       return AlertDialog(
//         title: Text("Make a Choice"),
//         content: SingleChildScrollView(
//           child: ListBody(
//             children: [
//
//               GestureDetector(
//                 child: Text('Gallery'),
//                 onTap: () {
//                   Future(()async{
//                     await _openGallery(context);
//                     Future.delayed(const Duration(seconds: 2));
//                     Navigator.push(context, MaterialPageRoute(builder: (context) => ProcessedImageScreen(imgPath)));
//                   });
//                 },
//               ),
//               Padding(
//                 padding: EdgeInsets.all(8.0),
//               ),
//               GestureDetector(
//                 child: Text('Camera'),
//                 onTap: () {
//                   _openCamera(context);
//                 },
//               )
//             ],
//           ),
//         ),
//       );
//     });
//   }
//
//   Widget _decideImageView()
//   {
//     if(imageFile == null)
//       return Text("No Image Selected");
//     else
//       return Image.file(imageFile!,width: 400,height: 400,);
//   }
//
//
//   @override
//   Widget build(BuildContext context) {
//     return Scaffold(
//       appBar: AppBar(
//         title :Text("Main Screen"),
//       ),
//       body: Container(
//         child: Center(
//           child: Column(
//             mainAxisAlignment: MainAxisAlignment.spaceAround,
//             children: [
//               _decideImageView(),
//               RaisedButton(
//                 onPressed:() {
//                   _showChoiceDialog(context);
//                 },
//                 child: Text("Select Image"),
//               ),
//               RaisedButton(
//                 onPressed:() {
//                   Navigator.push(context, MaterialPageRoute(builder: (context) => CompareNumbersScreen()));
//                 },
//                 child: Text("Compare Numbers"),
//               )
//
//             ],
//           ),
//         ),
//       ),
//     );
//   }
// }
