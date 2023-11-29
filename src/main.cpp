#include <Arduino.h>
#include "NeuralNetwork.h"
#include "backend/input_cam.h"
#include "backend/image.h"

NeuralNetwork *nn;

void setup()
{
  Serial.begin(115200);
  nn = new NeuralNetwork();

  Serial.println("Model Initialized");

  if (psramInit())
  {
    Serial.println("\nPSRAM is correctly initialized");
  }
  else
  {
    Serial.println("PSRAM not available");
  }

  int time = 3;

  Serial.printf("Waiting %d seconds for camera to boot", time);

  for (int i = 0; i < time; i++)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\n\n");
}

void loop()
{
  // int input_dims = 96 * 96 * 3;
  // uint8_t *input_data;

  // Serial.println("Getting camera data...");
  // for (int i = 0; i < input_dims; i++)
  // {
  //   input_data[i] = i;
  // }

  // input_data = get_cam();

  Serial.println("Copying input data to tensors");
  // for (int i = 0; i < 96 * 96 * 3; i++)
  // {
  //   Serial.printf("%d ", image[i]);
  // }

  float input_single_data = nn->getInputBuffer(image);

  // Serial.println(input_single_data);

  // nn->getInputBuffer()[0] = number1;
  // nn->getInputBuffer()[1] = number2;

  Serial.println("Predicting...");

  uint8_t *result = nn->predict();

  // const char *expected = number2 > number1 ? "True" : "False";

  // const char *predicted = result > 0.5 ? "True" : "False";

  // Serial.printf("%.2f %.2f - result %.2f - Expected %s, Predicted %s\n", number1, number2, result, expected, predicted);
  // Serial.println(result[0]);

  for (int i = 0; i < 10; i++)
  {
    Serial.printf("%d ", result[i]);
  }
  // Serial.println(*result);
  Serial.println("\n");
}