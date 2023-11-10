/*
  IMU Classifier
  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU, once enough samples are read, it then uses a
  TensorFlow Lite (Micro) model to try to classify the movement as a known gesture.
  Note: The direct use of C/C++ pointers, namespaces, and dynamic memory is generally
        discouraged in Arduino examples, and in the future the TensorFlowLite library
        might change to make the sketch simpler.
  The circuit:
  - Arduino Nano 33 BLE or Arduino Nano 33 BLE Sense board.
  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry
  This example code is in the public domain.
*/

#include <Arduino_LSM9DS1.h>

#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>

#include "model.h"

const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 119;

int samplesRead = numSamples;

// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

// array to map gesture index to a name
const char* GESTURES[] = {
  "YES",
  "NO"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize the IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // print out the samples rates of the IMUs
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();

  Serial.println("BIENVENUE à ce Quiz, vous repondrez aux différentes questions en faisant des gestes correspondants YES✓ NO X"); // Message de bienvenue

  Serial.println();

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;

   // Ask the first question
  askQuestion("Are you ready? (veuillez répondre YES pour pouvoir débuter)");

  // Check the response
  if (!waitForUserResponse()) {
    Serial.println("You are not ready. vous avez choisi no.");
    askQuestion("Are you ready? (veuillez répondre YES pour pouvoir débuter)");

    return;
  }

  askQuestion("Do you agree? (YES/NO)");
  askQuestion("Is it sunny today? (YES/NO)");
  askQuestion("Have you had lunch? (YES/NO)");
  // wait for significant motion
}

void askQuestion(const char* ques) {  
  float aX, aY, aZ, gX, gY, gZ;

   Serial.println(ques);

  while (samplesRead == numSamples) {
    if (IMU.accelerationAvailable()) {
      // read the acceleration data
      IMU.readAcceleration(aX, aY, aZ);

      // sum up the absolutes
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      // check if it's above the threshold
      if (aSum >= accelerationThreshold) {
        // reset the sample read count
        samplesRead = 0;
        break;
      }
    }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    // check if new acceleration AND gyroscope data is available
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      // read the acceleration and gyroscope data
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // normalize the IMU data between 0 to 1 and store in the model's
      // input tensor
      tflInputTensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

      samplesRead++;

      if (samplesRead == numSamples) {
        // Run inferencing
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk) {
          Serial.println("Invoke failed!");
          while (1);
          return;
        }
         
         int predictedGesture = -1;
        float maxConfidence = 0.0;
        // Loop through the output tensor values from the model
        

        for (int i = 0; i < NUM_GESTURES; i++) {
          /*Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 6);*/
       
          if (tflOutputTensor->data.f[i] > maxConfidence) {
            maxConfidence = tflOutputTensor->data.f[i];
            predictedGesture = i;
          }
        
        }
        Serial.print("vous avez répondu: ");
        Serial.println(GESTURES[predictedGesture]);
        Serial.println();

      /*for (int i = 0; i < NUM_GESTURES; i++) {
      
        Serial.println("Vérification: ");
        Serial.print(GESTURES[i]);
        Serial.print(": ");
        Serial.println(tflOutputTensor->data.f[i], 6);
      }*/

        Serial.println();
      }
    }
  }
}


bool waitForUserResponse() {
  // Wait for the user's response
  float aX, aY, aZ, gX, gY, gZ;
  while (samplesRead == numSamples) {
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(aX, aY, aZ);
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      if (aSum >= accelerationThreshold) {
        samplesRead = 0;
        break;
      }
    }
  }

  while (samplesRead < numSamples) {
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);
      tflInputTensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

      samplesRead++;

      if (samplesRead == numSamples) {
        // Run inferencing
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk) {
          Serial.println("Invoke failed!");
          while (1);
          return false;
        }

        // Determine the gesture based on the output tensor values
        int predictedGesture = -1;
        float maxConfidence = 0.0;
        for (int i = 0; i < NUM_GESTURES; i++) {
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 6);

          if (tflOutputTensor->data.f[i] > maxConfidence) {
            maxConfidence = tflOutputTensor->data.f[i];
            predictedGesture = i;
          }
        }

        // Check if the user responded with "YES"
        return (predictedGesture == 0);
      }
    }
  }

  // Default to false if something goes wrong
  return false;
}




/*void askQuestion(int gestureIndex) {
 // Serial.print("Question based on gesture '");
  
 // Serial.print(GESTURES[gestureIndex]);
 // Serial.println("':");
  //Serial.println(gestureIndex);

  switch (gestureIndex) {
    case 0:  // Gesture: YES
      //Serial.println("Do you agree? (YES/NO)");
      Serial.println("vous avez choisi yes");
      break;

    case 1:  // Gesture: NO
      //Serial.println("Do you disagree? (YES/NO)");
      Serial.println("vous avez choisi no");
      break;

    default:
      Serial.println("Unknown gesture, ask a general question.");
      Serial.println("Do you have any comments? (YES/NO)");
      break;
  }}


  void Question(askQuestion) {

    Serial.println("tu es beau?");

    switch (askQuestion) {
    case 0:  // Gesture: YES
      //Serial.println("Do you agree? (YES/NO)");
      Serial.println("le ciel est bleu?");
      break;

    case 1:  // Gesture: NO
      //Serial.println("Do you disagree? (YES/NO)");
      Serial.println("ru  es blanc?");
      break;}
      }
  // Wait for the user's response*/
