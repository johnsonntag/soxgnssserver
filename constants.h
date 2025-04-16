#ifndef CONSTANTS_H
#define CONSTANTS_H

#endif // CONSTANTS_H

// General constants
#define PI (4.0*atan((double)(1.0)))
#define DEG2RAD (PI/180.0)
#define RAD2DEG (180.0/PI)
#define RAD2NM  (180.0*60.0/PI)
#define RAD2KM  (RAD2NM*6076.1*12.0*2.54/100.0/1000.0)
#define NM2KM (6076.1*12.0*2.54/100.0/1000.0)
#define KM2NM 1.0/(NM2KM)
#define FT2M (12.0*2.54/100.0)  // divide by 3,28
#define M2FT (100.0/2.54/12.0)  // multiply by 3.28
#define AE 6378137.0
#define FLAT (1.0/298.257223563)

// General
#define USBGNSSDEVICE "/dev/gnss"
