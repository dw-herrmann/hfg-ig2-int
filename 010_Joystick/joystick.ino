#define joyX A0
#define joyY A1
int xValue;
int yValue;

void setup()
{
    Serial.begin(9600);
    Serial.println("test")
}

void loop()
{
    // put your main code here, to run repeatedly:
    xValue = analogRead(joyX);
    yValue = analogRead(joyY);

    //print the values with to plot or view
    Serial.print("Hallo");
    Serial.print(xValue);
    Serial.print("\t");
    Serial.println(yValue);
}