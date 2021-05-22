#define joyX A0
#define joyY A1

class Input
{
public:
    int posDefault[2];      // startpunkt bei laden, um ungenaue Startwerte auszugleichen

    int pos[2];             // positionsdaten - arrays müssen direkt mit Inhaltsmenge angegeben werden
    int posPercentage[2];   // position als %

    int angle;              // Winkel
};

int debug = 500;

Input input; // erstellt Objekt aus Klasse

void setup()
{
    Serial.begin(9600);
    Serial.println("test");

    // Eingabe-Werte Ausgangszustand feststellen
    input.posDefault[0] = analogRead(joyX);
    input.posDefault[1] = analogRead(joyY);
}

void loop()
{
    // Eingabe-Werte empfangen
    input.pos[0] = analogRead(joyX);
    input.pos[1] = analogRead(joyY);

    
    input.posPercentage[0] = map(input.pos[0], input.posDefault[0], 1023, 0, 100);
    input.posPercentage[1] = map(input.pos[1], input.posDefault[1], 1023, 0, 100);


    // Winkelberechnung
    // input.angle = atan2(
    //     input.posDefault[0] - input.pos[0], 
    //     input.posDefault[1] - input.pos[1]
    // );
    input.angle = atan2(
        input.posPercentage[0], 
        input.posPercentage[1]
    ) * 180 / PI;



    //print the values with to plot or view
    Serial.print(input.pos[0]);
    Serial.print(" ");
    Serial.print(input.pos[1]);
    Serial.print(" ");
    Serial.print(input.angle);
    Serial.println("°");


    delay(100);
}