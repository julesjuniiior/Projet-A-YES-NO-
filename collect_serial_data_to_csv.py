import serial
import csv

# Serial port configuration
SERIAL_PORT = 'com5'  # Replace with your serial port
BAUD_RATE = 9600  # Replace with your baud rate
numSamples = 119
Number_of_gestures = 0

# Open the serial port
with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
    # Open the CSV file to which you want to save the data
    with open('output.csv', 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)

        # Write the header
        writer.writerow(['aX', 'aY', 'aZ', 'gX', 'gY', 'gZ'])

        print("Starting data collection... Press Ctrl+C to stop.")

        samples_collected = 0  # Counter for the number of samples written

        try:
            # Keep reading data until you manually stop the program
            while True:
                line = ser.readline().decode('utf-8').strip()  # Read a line from the serial port

                # If the line is not empty and not the header, write it to the CSV
                if line and not line.startswith('aX,aY,aZ,gX,gY,gZ'):
                    values = line.split(',')  # Split the line into values
                    if len(values) == 6:  # Check we have 6 values, as expected
                        writer.writerow(values)  # Write to CSV
                        samples_collected += 1  # Increment the counter
                        # Print the message when the number of samples reaches numSamples
                        if samples_collected == numSamples:
                            print(f"{numSamples} samples have been saved.")
                            print("Data collection complete for one gesture.")
                            samples_collected = 0
                            Number_of_gestures += 1
                            print(f"Number of gestures: {Number_of_gestures}")
                    else:
                        print(f"Unexpected line format: {line}")
                elif line.startswith('aX,aY,aZ,gX,gY,gZ'):
                    print("Header detected, skipping...")

        except KeyboardInterrupt:
            # When you manually stop the program, it will print this message
            print("Data collection stopped by user.")

print("Data collection is complete. The data has been saved to 'output.csv'.")
