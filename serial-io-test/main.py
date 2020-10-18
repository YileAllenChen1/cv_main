import serial
from tkinter import *
import threading

SERIAL_PORT = '/dev/ttyACM0'  # Change this to be the serial port of the board

LED_CALLBACKS = [lambda:adjustLED(0),lambda:adjustLED(1),lambda:adjustLED(2),lambda:adjustLED(3),lambda:adjustLED(4),
                 lambda:adjustLED(5),lambda:adjustLED(6),lambda:adjustLED(7),lambda:adjustLED(8),lambda:adjustLED(9)]
LED_VARIABLES = []
BUZZER_ENABLED = None
BUZZER_BOX = None
BUTTON_LABEL = None

def buildWindow():
    global LED_VARIABLES, BUZZER_ENABLED, BUZZER_BOX, LED_CALLBACKS, BUTTON_LABEL
    window = Tk()
    window.title('CV to DJI Serial Demo')

    mainFrame = Frame(window)
    mainFrame.grid(row=0, column=0, padx=10, pady=10)

    # Initialize Frames
    ledFrame = LabelFrame(mainFrame, text='LED Control')
    ledFrame.grid(column=0, row=0, columnspan=2, sticky='ew')
    buzzerFrame = LabelFrame(mainFrame, text='Buzzer Control')
    buzzerFrame.grid(column=0, row=1)
    buttonFrame = LabelFrame(mainFrame, text='Button Status')
    buttonFrame.grid(column=1, row=1, sticky='ns')

    # Add LED Checkboxes
    ledCenterFrame = Frame(ledFrame)
    ledButtonNames = ['PG1', 'PG2', 'PG3', 'PG4', 'PG5', 'PG6', 'PG7', 'PG8', 'PF14', 'PE11']
    LED_VARIABLES = [IntVar() for x in range(len(ledButtonNames))]
    ledButtons = [Checkbutton(ledCenterFrame, command=callback, text=name, variable=var) for name, var, callback in zip(ledButtonNames, LED_VARIABLES, LED_CALLBACKS)]
    for buttonIndex in range(len(ledButtons)):
        column = buttonIndex // 4
        row = buttonIndex % 4
        ledButtons[buttonIndex].grid(row=row, column=column)
    ledCenterFrame.pack(side=TOP)

    # Add Buzzer Control
    buzzerLabel = Label(buzzerFrame, text='Frequency Value:')
    buzzerLabel.grid(column=0, row=0, columnspan=1)
    BUZZER_BOX = Spinbox(buzzerFrame, from_=5, to=60, width=8, command=adjustBuzzer)
    BUZZER_BOX.grid(column=0, row=1)
    #buzzerButton = Button(buzzerFrame, text="Update")
    #buzzerButton.grid(column=1, row=1)
    BUZZER_ENABLED = IntVar()
    buzzerEnabled = Checkbutton(buzzerFrame, text='Buzzer Enabled', variable=BUZZER_ENABLED, command=adjustBuzzer)
    buzzerEnabled.grid(column=0, row=2, columnspan=1)

    # Add Button Status
    BUTTON_LABEL = StringVar()
    buttonLabel = Label(buttonFrame, textvariable=BUTTON_LABEL, width=20)
    BUTTON_LABEL.set('Button not pressed')
    #buttonLabel.grid(row=0, column=0, sticky='ns')
    buttonLabel.pack(side=BOTTOM, expand=True)

    window.resizable(False, False)
    return window

def writeCommand(type: int, target: int, command: int):
    type, target, command = [value.to_bytes(1, 'big') for value in [type, target, command]]
    buffer = type + target + command
    SERIAL.write(buffer)

def adjustLED(index):
    writeCommand(1, index, 1 if LED_VARIABLES[index].get() == 1 else 0)

def adjustBuzzer():
    if BUZZER_ENABLED.get() == 0:
        # Buzzer disabled
        writeCommand(2, 0, 0)
    elif BUZZER_ENABLED.get() == 1:
       writeCommand(2, int(BUZZER_BOX.get()), 1)

def buttonListener():
    while True:
        value = SERIAL.read()
        if len(value) != 0:
            for byte in value:
                if byte == 1:
                    BUTTON_LABEL.set('Button pressed')
                elif byte == 0:
                    BUTTON_LABEL.set('Button not pressed')

SERIAL = serial.Serial
def main():
    with serial.Serial(SERIAL_PORT, timeout=3) as ser:
        global SERIAL
        SERIAL = ser
        window = buildWindow()
        threading._start_new_thread(buttonListener, ())
        window.mainloop()

if __name__ == '__main__':
    main()


