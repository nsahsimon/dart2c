import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:omr/omr.dart';

void main() {
  const MethodChannel channel = MethodChannel('omr');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await Omr.platformVersion, '42');
  });
}
