#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


TaskHandle_t blinkfunc, printFunc, deleteFunc, tempReadingFunc;

QueueHandle_t learningQueue;

void tempReading(void* pvParameters) {

	int reading;

	for (;;) {

		printf("Enter sensor value: ");

		if (scanf_s("%d", &reading) != 1) {

			printf("wrong datatype entered\n");

			return -1;

		}

		if (xQueueSend(learningQueue, &reading, 500) == pdPASS) {

			printf("Reading sent to queue\n");

			vTaskDelay(200 / portTICK_PERIOD_MS);

		}

		else {

			printf("Add to queue operation failed\n");
			vTaskDelay(200 / portTICK_PERIOD_MS);

		}
	}

}
void blinkLed(void* pvParameters) {

	int recieveQueue;

	while (1) {

		if (xQueueReceive(learningQueue, &recieveQueue, 500) == pdPASS) {

			printf("on\n");
			vTaskDelay(500 / portTICK_PERIOD_MS);
			printf("off\n");
			vTaskDelay(500 / portTICK_PERIOD_MS);

		}
	}
}

void printString(void* pvParameters) {
	for (;;) {

		printf("my name\n");
		vTaskDelay(100 / portTICK_PERIOD_MS);


	}
}

void deleteTaskOne(void* pvParameters) {

	int i = 0;

	for (;;) {

		while (i < 5) {

			printf("I is currently number %d\n", i);
			i++;
			vTaskDelay(1000 / portTICK_PERIOD_MS);

		}

		printf("Deleting task 1\n");
		vTaskDelete(printFunc);
		printf("Task one deleted\n");
		vTaskDelay(2000 / portTICK_PERIOD_MS);

		while (i >= 0) {

			printf("I is currently number %d\n", i);
			i--;
			vTaskDelay(1000 / portTICK_PERIOD_MS);

		}

		printf("Deleting the deleting function\n");
		vTaskDelete(deleteFunc);

	}

}

void main() {

	learningQueue = xQueueCreate(5, sizeof(int));

	xTaskCreate(tempReading, "funcToSimulateSensorValue", 500, NULL, 3, &tempReadingFunc);
	xTaskCreate(blinkLed, "funcToBlinkLed", 500, NULL, 2, &blinkfunc);
	xTaskCreate(printString, "funcToPrintString", 500, NULL, 1, &printFunc);
	xTaskCreate(deleteTaskOne, "funcToDeletTask", 500, NULL, 0, &deleteFunc);
	vTaskStartScheduler();

	for (;;) {}

}