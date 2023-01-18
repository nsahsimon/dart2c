import "package:flutter/material.dart";
import "package:modal_progress_hud_nsn/modal_progress_hud_nsn.dart";
import "package:auto_size_text/auto_size_text.dart";
import "dart:ffi";
import "dart:io";
import "package:dart2c/dart2c.dart";
class CompareNumbersScreen extends StatefulWidget {
  @override
  _CompareNumbersScreenState createState() => _CompareNumbersScreenState();
}

class _CompareNumbersScreenState extends State<CompareNumbersScreen> {

  TextEditingController num1Controller = TextEditingController(text: "0");
  TextEditingController num2Controller = TextEditingController(text: "0");
  int sum = 0;
  String returnedText = "Place the returned text here", num1Str = "0", num2Str = "0";
  bool isLoading = false;
  void startLoading() {
    setState((){
      isLoading = true;
    });
  }

  void stopLoading() {
    setState((){
      isLoading = false;
    });
  }

  Future<void> compute() async {
   int num1 = int.parse(num1Controller.text);
   int num2 = int.parse(num2Controller.text);
   int result = await Native().nativeAdd(num1: num1, num2: num2);
    setState((){
       sum = result;//todo: change this to the value returned by the c++ functions
      returnedText = "Result is ${result}"; //todo: change this to the value returned by the c++ function
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Compare two numbers"),
      ),
      body: ModalProgressHUD(
        inAsyncCall: isLoading,
        child: Column(
          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [

            TextFormField(controller: num1Controller, decoration: InputDecoration(labelText: "Enter Number 1"),keyboardType: TextInputType.number, onChanged: (text){ setState((){ num1Str = text;});} ),
            TextFormField(controller: num2Controller, decoration: InputDecoration(labelText: "Enter Number 2"),keyboardType: TextInputType.number, onChanged: (text){ setState((){ num2Str = text;});} ),
            Center(
              child: AutoSizeText(num1Str + " + " + num2Str + " = " + sum.toString(), maxLines: 1), //todo: display the result here
            ),
            Center(
              child: AutoSizeText(returnedText, maxLines: 1) //todo: Display the returned text here
            ),

            Center(
              child: GestureDetector(
                child:const Text("Calculate", style: TextStyle(color: Colors.green)),
                onTap: () {
                  compute();
                }
              )
            )
          ]
        ),
      )
    );
  }
}
