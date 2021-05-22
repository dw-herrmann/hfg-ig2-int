void inputFunctionJoystick()
{
    // Eingabe-Werte empfangen
    input.pos[0] = analogRead(joyX);
    input.pos[1] = analogRead(joyY);

    // Werte in % umrechnen
    input.posPercentage[0] = map(input.pos[0], input.posDefault[0], 1023, 0, 100);
    input.posPercentage[1] = map(input.pos[1], input.posDefault[1], 1023, 0, 100);

    // Winkel herausfinden
    input.angle = -atan2(
                      input.posPercentage[0],
                      input.posPercentage[1]) *
                  180 / PI;

    // Krafteinwirkung aus Joystick herausfinden
    input.force = (fabs(input.posPercentage[0]) + fabs(input.posPercentage[0])) / 2;
}

void inputFunctionButton()
{
    // Button gepresst oder nicht
    input.buttonSensor = digitalRead(button);
    Serial.print("sensorVal: ");
    Serial.println(input.buttonSensor);
    Serial.print("ledState: ");
    Serial.println(input.buttonState);

    // falls button gedrückt und state aus
    if (input.buttonSensor == 1 && input.buttonState == 0)
    {
        input.buttonState = 1;
        Serial.print("button pressed");
    }

    // falls button nicht gedrückt und state an
    else if (input.buttonSensor == 0 && input.buttonState == 1)
    {
        input.buttonState = 0;
    }
}