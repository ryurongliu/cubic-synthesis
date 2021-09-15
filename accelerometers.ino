// Basic demo for accelerometer & gyro readings from Adafruit
// LSM6DSO32 sensor

#include <Adafruit_LSM6DSO32.h>

// For SPI mode, we need a CS pin
#define LSM_CS_1 9
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK_1 13   //SCL
#define LSM_MISO_1 12 //DO
#define LSM_MOSI_1 10 //SDA


#define LSM_CS_2 3
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK_2 6   //SCL
#define LSM_MISO_2 4 //DO
#define LSM_MOSI_2 5 //SDA

Adafruit_LSM6DSO32 dso32_1;
Adafruit_LSM6DSO32 dso32_2;
void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit LSM6DSO32 test!");

  //check ds032_1
  if (!dso32_1.begin_SPI(LSM_CS_1, LSM_SCK_1, LSM_MISO_1, LSM_MOSI_1)) {
     Serial.println("Failed to find LSM6DSO32_1 chip");
    while (1) {
      delay(10);
    }  
  }
  
  Serial.println("LSM6DSO32_1 Found!");

  //check ds032_2
  if (!dso32_2.begin_SPI(LSM_CS_2, LSM_SCK_2, LSM_MISO_2, LSM_MOSI_2)) {
     Serial.println("Failed to find LSM6DSO32_2 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("LSM6DSO32_2 Found!");



  dso32_1.setAccelRange(LSM6DSO32_ACCEL_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (dso32_1.getAccelRange()) {
  case LSM6DSO32_ACCEL_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case LSM6DSO32_ACCEL_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case LSM6DSO32_ACCEL_RANGE_16_G:
    Serial.println("+-16G");
    break;
  case LSM6DSO32_ACCEL_RANGE_32_G:
    Serial.println("+-32G");
    break;
  }

  dso32_2.setAccelRange(LSM6DSO32_ACCEL_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (dso32_2.getAccelRange()) {
  case LSM6DSO32_ACCEL_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case LSM6DSO32_ACCEL_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case LSM6DSO32_ACCEL_RANGE_16_G:
    Serial.println("+-16G");
    break;
  case LSM6DSO32_ACCEL_RANGE_32_G:
    Serial.println("+-32G");
    break;
  }

  // dso32.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS );
  Serial.print("Gyro range set to: ");
  switch (dso32_1.getGyroRange()) {
  case LSM6DS_GYRO_RANGE_125_DPS:
    Serial.println("125 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_250_DPS:
    Serial.println("250 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_500_DPS:
    Serial.println("500 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_1000_DPS:
    Serial.println("1000 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_2000_DPS:
    Serial.println("2000 degrees/s");
    break;
  case ISM330DHCX_GYRO_RANGE_4000_DPS:
    break; // unsupported range for the DSO32
  }

  Serial.print("Gyro range set to: ");
  switch (dso32_2.getGyroRange()) {
  case LSM6DS_GYRO_RANGE_125_DPS:
    Serial.println("125 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_250_DPS:
    Serial.println("250 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_500_DPS:
    Serial.println("500 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_1000_DPS:
    Serial.println("1000 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_2000_DPS:
    Serial.println("2000 degrees/s");
    break;
  case ISM330DHCX_GYRO_RANGE_4000_DPS:
    break; // unsupported range for the DSO32
  }

  // dso32.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
  Serial.print("Accelerometer data rate set to: ");
  switch (dso32_1.getAccelDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }

   // dso32.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
  Serial.print("Accelerometer data rate set to: ");
  switch (dso32_2.getAccelDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }

  // dso32.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
  Serial.print("Gyro data rate set to: ");
  switch (dso32_1.getGyroDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }

  // dso32.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
  Serial.print("Gyro data rate set to: ");
  switch (dso32_2.getGyroDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }
}

void loop() {

  //  /* Get a new normalized sensor event */
  sensors_event_t accel_1;
  sensors_event_t gyro_1;
  sensors_event_t temp_1;
  dso32_1.getEvent(&accel_1, &gyro_1, &temp_1);

  sensors_event_t accel_2;
  sensors_event_t gyro_2;
  sensors_event_t temp_2;
  dso32_2.getEvent(&accel_2, &gyro_2, &temp_2);

  //send formatted as: "temp1 accel_1x accel_1y accel_1z gyro_1x gyro_1y gyro_1z temp2 accel_2x accel_2y accel_2z gyro_2x gyro_2y gyro_2z"

  
  Serial.print(temp_1.temperature);
  Serial.print(" ");
  Serial.print(accel_1.acceleration.x);
  Serial.print(" ");
  Serial.print(accel_1.acceleration.y);
  Serial.print(" ");
  Serial.print(accel_1.acceleration.z);
  Serial.print(" ");
  Serial.print(gyro_1.gyro.x);
  Serial.print(" ");
  Serial.print(gyro_1.gyro.y);
  Serial.print(" ");
  Serial.print(gyro_1.gyro.z);
  Serial.print(" ");
  Serial.print(temp_2.temperature);
  Serial.print(" ");
  Serial.print(accel_2.acceleration.x);
  Serial.print(" ");
  Serial.print(accel_2.acceleration.y);
  Serial.print(" ");
  Serial.print(accel_2.acceleration.z);
  Serial.print(" ");
  Serial.print(gyro_2.gyro.x);
  Serial.print(" ");
  Serial.print(gyro_2.gyro.y);
  Serial.print(" ");
  Serial.print(gyro_2.gyro.z);
  Serial.println("");

/*
  Serial.println("Accelerometer 1: ");
  Serial.print("\t\tTemperature ");
  Serial.print(temp_1.temperature);
  Serial.println(" deg C");


  Serial.print("\t\tAccel X: ");
  Serial.print(accel_1.acceleration.x);
  Serial.print(" \tY: ");
  Serial.print(accel_1.acceleration.y);
  Serial.print(" \tZ: ");
  Serial.print(accel_1.acceleration.z);
  Serial.println(" m/s^2 ");


  Serial.print("\t\tGyro X: ");
  Serial.print(gyro_1.gyro.x);
  Serial.print(" \tY: ");
  Serial.print(gyro_1.gyro.y);
  Serial.print(" \tZ: ");
  Serial.print(gyro_1.gyro.z);
  Serial.println(" radians/s ");
  Serial.println();

  Serial.println("Accelerometer 2: ");
  Serial.print("\t\tTemperature ");
  Serial.print(temp_2.temperature);
  Serial.println(" deg C");

  Serial.print("\t\tAccel X: ");
  Serial.print(accel_2.acceleration.x);
  Serial.print(" \tY: ");
  Serial.print(accel_2.acceleration.y);
  Serial.print(" \tZ: ");
  Serial.print(accel_2.acceleration.z);
  Serial.println(" m/s^2 ");


  Serial.print("\t\tGyro X: ");
  Serial.print(gyro_2.gyro.x);
  Serial.print(" \tY: ");
  Serial.print(gyro_2.gyro.y);
  Serial.print(" \tZ: ");
  Serial.print(gyro_2.gyro.z);
  Serial.println(" radians/s ");
  Serial.println();
*/  
  delay(100);

  //  // serial plotter friendly format

  //  Serial.print(temp.temperature);
  //  Serial.print(",");

  //  Serial.print(accel.acceleration.x);
  //  Serial.print(","); Serial.print(accel.acceleration.y);
  //  Serial.print(","); Serial.print(accel.acceleration.z);
  //  Serial.print(",");

  // Serial.print(gyro.gyro.x);
  // Serial.print(","); Serial.print(gyro.gyro.y);
  // Serial.print(","); Serial.print(gyro.gyro.z);
  // Serial.println();
  //  delayMicroseconds(10000);
}
