#include <TensorFlowLite_ESP32.h>
#include <Arduino.h>

#include "NeuralNetwork.h"
// #include "models/unet_2epochs.h"
// #include "models/modelData.h"
// #include "models/testing.h"
// #include "models/lightweightUnet_withAdd.h"
// #include "models/lightweigthUnet_2layerModel_128x128.h"
#include "models/lightweight_resnet_1epoch_96imgsize.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

// const int kArenaSize = 1200000;
const int kArenaSize = 2400000;

NeuralNetwork::NeuralNetwork()
{
    error_reporter = new tflite::MicroErrorReporter();

    // model = tflite::GetModel(unet_2epochs_h5);
    // model = tflite::GetModel(modelData);
    // model = tflite::GetModel(lightweightUnet_withAdd);
    // model = tflite::GetModel(lightweigthUnet_2layerModel_128x128);
    model = tflite::GetModel(lightweight_resnet_1epoch_96imgsize);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "Model provided is schema version %d not equal to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        while (true)
            ;
    }
    // This pulls in the operators implementations we need
    resolver = new tflite::MicroMutableOpResolver<7>();
    resolver->AddFullyConnected();
    resolver->AddMul();
    resolver->AddAdd();
    resolver->AddConv2D();
    resolver->AddMaxPool2D();
    resolver->AddSoftmax();
    resolver->AddResizeNearestNeighbor();

    tensor_arena = (uint8_t *)malloc(kArenaSize);
    if (!tensor_arena)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "Could not allocate arena");
        while (true)
            ;
    }

    // Build an interpreter to run the model with.
    interpreter = new tflite::MicroInterpreter(
        model, *resolver, tensor_arena, kArenaSize, error_reporter);

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        while (true)
            ;
    }

    size_t used_bytes = interpreter->arena_used_bytes();
    TF_LITE_REPORT_ERROR(error_reporter, "Used bytes %d\n", used_bytes);

    // Obtain pointers to the model's input and output tensors.
    input = interpreter->input(0);
    output = interpreter->output(0);
}

uint8_t NeuralNetwork::getInputBuffer(const unsigned char *input_buffer)
{
    int input_buffer_size = input->bytes / sizeof(uint8_t);
    for (int i = 0; i < input_buffer_size; i++)
    {
        input->data.uint8[i] = input_buffer[i];
    }

    // Serial.print("Input datatype: ");
    // Serial.println(String(input->type));

    // Serial.print("Output datatype: ");
    // Serial.println(String(output->type));

    Serial.println("Input Data:");
    for (int i = 0; i < 10; i++)
    {
        Serial.printf("%d ", input->data.uint8[i]);
    }

    Serial.println();

    for (int i = 0; i < 10; i++)
    {
        Serial.printf("%d ", input->data.f[i]);
    }

    Serial.println("\n");

    return input->data.uint8[0];
}

uint8_t *NeuralNetwork::predict()
{
    TfLiteStatus invoke_status = interpreter->Invoke();

    if (invoke_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke() failed");
        while (true)
            ;
    }

    Serial.print("Input dimensions: ");
    for (int i = 0; i < 4; i++)
    {
        Serial.printf("%d ", input->dims->data[i]);
    }
    Serial.print("\nOutput dimensions: ");
    for (int i = 0; i < output->dims->size; i++)
    {
        Serial.printf("%d ", output->dims->data[i]);
    }

    Serial.println();

    uint8_t *out = new uint8_t[96 * 96 * 5];
    for (int i = 0; i < 96 * 96 * 5; i++)
    {
        out[i] = output->data.uint8[i];
    }

    for (int i = 0; i < 10; i++)
    {
        Serial.printf("%d \t", output->data.b[i]);
        Serial.printf("%d \t", output->data.uint8[i]);
        Serial.printf("%d \t", output->data.c128[i]);
        Serial.printf("%d \t", output->data.c64[i]);
        Serial.printf("%d \t", output->data.f16[i]);
        Serial.printf("%d \t", output->data.f64[i]);
        Serial.printf("%d \t", output->data.f[i]);
        Serial.printf("%d \t", output->data.i16[i]);
        Serial.printf("%d \t", output->data.i32[i]);
        Serial.printf("%d \t", output->data.i64[i]);
        Serial.printf("%d \t", output->data.int8[i]);
        Serial.printf("%d \t", output->data.raw[i]);
        Serial.printf("%d \t", output->data.raw_const[i]);
        Serial.printf("%d \t", output->data.u32[i]);
        Serial.printf("%d \t", output->data.u64[i]);
        Serial.printf("%d \t", output->data.ui16[i]);
        Serial.printf("%d \t", output->data.uint8[i]);
        Serial.printf("%d \t", output->data);
        Serial.println();
    }
    Serial.println();

    for (int i = 0; i < 96 * 96 * 5; i++)
    {
        Serial.printf("%d ", output->data.uint8[i]);
    }

    Serial.println();

    return out;
    // return output->data.f;
}