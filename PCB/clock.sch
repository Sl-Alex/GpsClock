EESchema Schematic File Version 2
LIBS:discrete
LIBS:holtek
LIBS:user_parts
LIBS:connectors
LIBS:power
LIBS:clock-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "noname.sch"
Date "1 sep 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 3800 2700 0    60   ~ 0
VCC
Text Label 3800 4500 0    60   ~ 0
SCL
Text Label 3800 4600 0    60   ~ 0
SDA
Text Label 5600 3700 0    60   ~ 0
ROW1
Text Label 5600 2700 0    60   ~ 0
COL0
Text Label 5600 2800 0    60   ~ 0
COL1
Text Label 5600 3000 0    60   ~ 0
COL3
Text Label 5600 3100 0    60   ~ 0
COL4
Text Label 5600 3200 0    60   ~ 0
COL5
Text Label 5600 3300 0    60   ~ 0
COL6
Text Label 5600 3400 0    60   ~ 0
COL7
Text Label 5600 3900 0    60   ~ 0
ROW3
Text Label 5600 3600 0    60   ~ 0
ROW0
Text Label 5600 2900 0    60   ~ 0
COL2
Text Label 5600 4000 0    60   ~ 0
ROW4
Text Label 5600 4200 0    60   ~ 0
ROW6
Text Label 5600 4300 0    60   ~ 0
ROW7
Text Label 5600 4100 0    60   ~ 0
ROW5
Text Label 5600 3800 0    60   ~ 0
ROW2
$Comp
L capacitor C1
U 1 1 5559CAAA
P 4050 2900
F 0 "C1" V 4100 2800 60  0000 C CNN
F 1 "capacitor" H 4200 2800 60  0001 C CNN
F 2 "discrete_SMD:0603_C" H 4050 2900 60  0001 C CNN
F 3 "" H 4050 2900 60  0000 C CNN
	1    4050 2900
	0    1    1    0   
$EndComp
$Comp
L diode D1
U 1 1 5559AEA0
P 6800 4600
F 0 "D1" H 6950 4700 60  0000 C CNN
F 1 "diode" H 7000 4500 60  0001 C CNN
F 2 "discrete_SMD:SOD-80" H 6850 3700 60  0001 C CNN
F 3 "" H 6850 3700 60  0000 C CNN
	1    6800 4600
	1    0    0    -1  
$EndComp
$Comp
L resistor R1
U 1 1 5559B080
P 7650 4600
F 0 "R1" H 8000 4500 60  0000 C CNN
F 1 "resistor" H 7800 4500 60  0001 C CNN
F 2 "discrete_SMD:0402_R" H 7650 4600 60  0001 C CNN
F 3 "" H 7650 4600 60  0000 C CNN
	1    7650 4600
	-1   0    0    1   
$EndComp
$Comp
L resistor R2
U 1 1 5559B0D1
P 7650 4800
F 0 "R2" H 8000 4700 60  0000 C CNN
F 1 "resistor" H 7800 4700 60  0001 C CNN
F 2 "discrete_SMD:0402_R" H 7650 4800 60  0001 C CNN
F 3 "" H 7650 4800 60  0000 C CNN
	1    7650 4800
	-1   0    0    1   
$EndComp
$Comp
L resistor R3
U 1 1 5559B125
P 7650 5000
F 0 "R3" H 8000 4900 60  0000 C CNN
F 1 "resistor" H 7800 4900 60  0001 C CNN
F 2 "discrete_SMD:0402_R" H 7650 5000 60  0001 C CNN
F 3 "" H 7650 5000 60  0000 C CNN
	1    7650 5000
	-1   0    0    1   
$EndComp
$Comp
L DM88-300S D2
U 1 1 5559B672
P 6400 2700
F 0 "D2" H 6550 2850 60  0000 C CNN
F 1 "DM88-300S" H 6950 2850 60  0000 C CNN
F 2 "user_parts:DM88-300S" H 6450 1800 60  0001 C CNN
F 3 "" H 6450 1800 60  0000 C CNN
	1    6400 2700
	1    0    0    -1  
$EndComp
$Comp
L DM88-300S D3
U 1 1 5559B6AA
P 7700 2700
F 0 "D3" H 7850 2850 60  0000 C CNN
F 1 "DM88-300S" H 8250 2850 60  0000 C CNN
F 2 "user_parts:DM88-300S" H 7750 1800 60  0001 C CNN
F 3 "" H 7750 1800 60  0000 C CNN
	1    7700 2700
	1    0    0    -1  
$EndComp
$Comp
L HT16K33-28 U1
U 1 1 53052BA2
P 4150 2700
F 0 "U1" H 4350 2850 60  0000 C CNN
F 1 "HT16K33-28" H 5100 2850 60  0000 C CNN
F 2 "SO:SO28" H 4200 1800 60  0001 C CNN
F 3 "" H 4200 1800 60  0000 C CNN
	1    4150 2700
	1    0    0    -1  
$EndComp
$Comp
L PLS1 X1
U 1 1 5559B63A
P 3450 2700
F 0 "X1" H 4050 2700 60  0000 C CNN
F 1 "VCC" H 3700 2700 60  0000 C CNN
F 2 "CONNECTORS:PLS1_SMD" H 3450 2700 60  0001 C CNN
F 3 "" H 3450 2700 60  0000 C CNN
	1    3450 2700
	-1   0    0    1   
$EndComp
$Comp
L PLS1 X2
U 1 1 5559B6A6
P 3450 3450
F 0 "X2" H 4050 3450 60  0000 C CNN
F 1 "GND" H 3700 3450 60  0000 C CNN
F 2 "CONNECTORS:PLS1_SMD" H 3450 3450 60  0001 C CNN
F 3 "" H 3450 3450 60  0000 C CNN
	1    3450 3450
	-1   0    0    1   
$EndComp
$Comp
L PLS1 X3
U 1 1 5559B703
P 3450 4500
F 0 "X3" H 4050 4500 60  0000 C CNN
F 1 "SCL" H 3700 4500 60  0000 C CNN
F 2 "CONNECTORS:PLS1_SMD" H 3450 4500 60  0001 C CNN
F 3 "" H 3450 4500 60  0000 C CNN
	1    3450 4500
	-1   0    0    1   
$EndComp
$Comp
L PLS1 X4
U 1 1 5559B743
P 3450 4600
F 0 "X4" H 4050 4600 60  0000 C CNN
F 1 "SDA" H 3700 4600 60  0000 C CNN
F 2 "CONNECTORS:PLS1_SMD" H 3450 4600 60  0001 C CNN
F 3 "" H 3450 4600 60  0000 C CNN
	1    3450 4600
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR01
U 1 1 5559B907
P 4050 3550
F 0 "#PWR01" H 4050 3300 50  0001 C CNN
F 1 "GND" H 4050 3400 50  0000 C CNN
F 2 "" H 4050 3550 60  0000 C CNN
F 3 "" H 4050 3550 60  0000 C CNN
	1    4050 3550
	1    0    0    -1  
$EndComp
$Comp
L capacitor_el C2
U 1 1 5559D571
P 3750 2900
F 0 "C2" V 3800 2800 60  0000 C CNN
F 1 "capacitor_el" H 3900 2800 60  0001 C CNN
F 2 "discrete_SMD:case_B" H 3750 2900 60  0001 C CNN
F 3 "" H 3750 2900 60  0000 C CNN
	1    3750 2900
	0    1    1    0   
$EndComp
Text Label 5600 4400 0    60   ~ 0
ROW8
Text Label 5600 4500 0    60   ~ 0
ROW9
Text Label 5600 4600 0    60   ~ 0
ROW10
Text Label 5600 4700 0    60   ~ 0
ROW11
Text Label 5600 4800 0    60   ~ 0
ROW12
Text Label 5600 4900 0    60   ~ 0
ROW13
Text Label 5600 5000 0    60   ~ 0
ROW14
Text Label 5600 5100 0    60   ~ 0
ROW15
Wire Wire Line
	3550 4500 4150 4500
Wire Wire Line
	3550 4600 4150 4600
Wire Wire Line
	4050 2700 4050 2900
Wire Wire Line
	3550 2700 4150 2700
Wire Wire Line
	4050 3200 4050 3550
Wire Wire Line
	3550 3450 4150 3450
Connection ~ 4050 2700
Connection ~ 4050 3450
Wire Wire Line
	7700 3000 7300 3000
Wire Wire Line
	7300 3100 7700 3100
Wire Wire Line
	7700 3200 7300 3200
Wire Wire Line
	7300 3300 7700 3300
Wire Wire Line
	7700 3400 7300 3400
Wire Wire Line
	7300 3600 7700 3600
Wire Wire Line
	7300 3700 7700 3700
Wire Wire Line
	7300 3800 7700 3800
Wire Wire Line
	7700 3900 7300 3900
Wire Wire Line
	7300 4000 7700 4000
Wire Wire Line
	7700 4100 7300 4100
Wire Wire Line
	7300 4200 7700 4200
Wire Wire Line
	7700 4300 7300 4300
Wire Wire Line
	3750 2900 3750 2700
Connection ~ 3750 2700
Wire Wire Line
	3750 3200 3750 3450
Connection ~ 3750 3450
Wire Wire Line
	5550 3600 5900 3600
Wire Wire Line
	5550 2700 5900 2700
Wire Wire Line
	7300 2900 7700 2900
Wire Wire Line
	7700 2800 7300 2800
Wire Wire Line
	6000 3000 6400 3000
Wire Wire Line
	7700 2700 7300 2700
Wire Wire Line
	5550 2800 5900 2800
Wire Wire Line
	6000 3200 6400 3200
Wire Wire Line
	5550 2900 5900 2900
Wire Wire Line
	6000 3300 6400 3300
Wire Wire Line
	5550 3000 5900 3000
Wire Wire Line
	6000 2700 6400 2700
Wire Wire Line
	5900 3100 5550 3100
Wire Wire Line
	6000 2800 6400 2800
Wire Wire Line
	5550 3200 5900 3200
Wire Wire Line
	6000 3400 6400 3400
Wire Wire Line
	6000 2900 6400 2900
Wire Wire Line
	5900 3300 5550 3300
Wire Wire Line
	6400 3100 6000 3100
Wire Wire Line
	5900 3400 5550 3400
Wire Wire Line
	5550 5100 5900 5100
Wire Wire Line
	6000 3800 6400 3800
Wire Wire Line
	6400 4100 6000 4100
Wire Wire Line
	5550 5000 5900 5000
Wire Wire Line
	5550 4900 5900 4900
Wire Wire Line
	6000 4300 6400 4300
Wire Wire Line
	5550 4800 5900 4800
Wire Wire Line
	6000 4200 6400 4200
Wire Wire Line
	5550 4700 5900 4700
Wire Wire Line
	6000 4000 6400 4000
Wire Wire Line
	6000 3700 6400 3700
Wire Wire Line
	5550 4500 5900 4500
Wire Wire Line
	6000 3900 6400 3900
Wire Wire Line
	6000 3600 6400 3600
Wire Wire Line
	5550 4300 5900 4300
Wire Wire Line
	5550 4200 5900 4200
Wire Wire Line
	5900 4100 5550 4100
Wire Wire Line
	5900 4000 5550 4000
Wire Wire Line
	5550 3900 5900 3900
Wire Wire Line
	5550 4400 5900 4400
Wire Wire Line
	5550 4600 5900 4600
Wire Wire Line
	5550 3700 5900 3700
Wire Wire Line
	5550 3800 5900 3800
Text Label 6050 3000 0    60   ~ 0
COL0
Text Label 7350 3000 0    60   ~ 0
COL0
Text Label 6450 4600 0    60   ~ 0
COL0
Text Label 6050 3200 0    60   ~ 0
COL1
Text Label 7350 3200 0    60   ~ 0
COL1
Text Label 6050 3300 0    60   ~ 0
COL2
Text Label 6050 2700 0    60   ~ 0
COL3
Text Label 6050 2800 0    60   ~ 0
COL4
Text Label 6050 3400 0    60   ~ 0
COL5
Text Label 6050 2900 0    60   ~ 0
COL6
Text Label 6050 3100 0    60   ~ 0
COL7
Text Label 7350 2700 0    60   ~ 0
COL3
Text Label 7350 2800 0    60   ~ 0
COL4
Text Label 7350 2900 0    60   ~ 0
COL6
Text Label 7350 3100 0    60   ~ 0
COL7
Text Label 7350 3300 0    60   ~ 0
COL2
Text Label 7350 3400 0    60   ~ 0
COL5
Text Label 7350 4000 0    60   ~ 0
ROW0
Text Label 7350 4200 0    60   ~ 0
ROW1
Text Label 7350 4300 0    60   ~ 0
ROW2
Text Label 7350 4100 0    60   ~ 0
ROW3
Text Label 7350 3800 0    60   ~ 0
ROW4
Text Label 7350 3600 0    60   ~ 0
ROW5
Text Label 7350 3900 0    60   ~ 0
ROW6
Text Label 7350 3700 0    60   ~ 0
ROW7
Text Label 6050 3700 0    60   ~ 0
ROW8
Text Label 6050 3900 0    60   ~ 0
ROW9
Text Label 6050 3600 0    60   ~ 0
ROW10
Text Label 6050 4000 0    60   ~ 0
ROW11
Text Label 6050 4200 0    60   ~ 0
ROW12
Text Label 6050 4300 0    60   ~ 0
ROW13
Text Label 6050 4100 0    60   ~ 0
ROW14
Text Label 6050 3800 0    60   ~ 0
ROW15
Wire Wire Line
	6800 4600 6400 4600
Wire Wire Line
	7100 4600 7300 4600
Wire Wire Line
	7200 4600 7200 5000
Wire Wire Line
	7200 5000 7300 5000
Wire Wire Line
	7300 4800 7200 4800
Connection ~ 7200 4800
Connection ~ 7200 4600
Wire Wire Line
	7650 4600 8050 4600
Wire Wire Line
	8050 4800 7650 4800
Wire Wire Line
	8050 5000 7650 5000
Text Label 7750 4600 0    60   ~ 0
ROW0
Text Label 7750 4800 0    60   ~ 0
ROW1
Text Label 7750 5000 0    60   ~ 0
ROW2
$EndSCHEMATC
