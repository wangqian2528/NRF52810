#ifndef _PRESSURE_H
#define _PRESSURE_H

void twi_init(void);
void pressure_measure(void);
float get_pressure_value(void);
float get_tempture_value(void);

#endif
