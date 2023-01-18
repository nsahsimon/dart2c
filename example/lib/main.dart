import 'package:flutter/material.dart';
import "camera_screen.dart";
import "cmp_numbers.dart";
import "package:dart2c/dart2c.dart";
void main() {
  Native().initialize();
  runApp(MaterialApp(
    title : "Camera App",
    home : CompareNumbersScreen(),
  ));
}
