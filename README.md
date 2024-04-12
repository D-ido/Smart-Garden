Management Summary:
Das Smart-Garden-Projekt ist eine IoT-Anwendung mit dem Hauptfokus auf die effiziente Aufzucht und Ernte von Gemüse und Pflanzen für den Heimgebrauch. Es nutzt IoT-Technologie, um die Bewässerung, Beleuchtung und Überwachung von Pflanzen automatisch zu steuern.

Funktionsweise:
WLAN-Konnektivität: Das System verbindet sich mit einem WLAN-Netzwerk, um MQTT-Kommunikation zu ermöglichen und Daten zu übertragen.
MQTT-Kommunikation: Es verwendet das MQTT-Protokoll, um mit einem lokalen Broker (Raspberry-Pi) zu kommunizieren. Dadurch können verschiedene Aspekte des Gemüsegartens über Nachrichten (Topics) wie Schalterstatus, Lichtsteuerung und Sensorausgabe gesteuert werden.
Sensoren:
Bodenfeuchtesensor: Überwacht den Feuchtigkeitsgehalt des Bodens.
Ultraschallsensor: Ermittelt den Wasserstand.
Echtzeituhr (RTC): Zeigt die aktuelle Zeit für zeitgesteuerte Aktionen an.
Aktoren: Relais für Licht und
Pumpe: Schaltet die Beleuchtung und die Bewässerungspumpe ein und aus.
Lüfter: Wird periodisch basierend auf Zeitintervallen aktiviert.
Funktionalität:
Automatische Bewässerung: Die Bewässerung wird durch den Bodenfeuchtesensor gesteuert, um optimale Feuchtigkeitsbedingungen für das Pflanzenwachstum zu gewährleisten.
Lichtsteuerung: Die Beleuchtung wird um 06.00 Uhr morgens automatisch eingeschaltet und um 18:00 uhr ausgeschaltet, um das Pflanzenwachstum zu fördern.
Gemüsezucht für Zuhause: Das System unterstützt die Aufzucht und Ernte von Gemüse und Pflanzen für den Hausgebrauch, wobei der Schwerpunkt auf der effizienten Nutzung von Ressourcen liegt.
Ziel und Nutzen:
Das Ziel dieses Projekts ist es, Nutzern eine einfache und effektive Möglichkeit zu bieten, eigenes Gemüse und eigenen Pflanzen zu Hause anzubauen und zu ernten. Durch die Automatisierung von Bewässerung und Beleuchtung wird der Anbau erleichtert und optimiert.
Zukünftige Erweiterungen:
Potenzielle Erweiterungen des Projekts könnten die Integration weiterer Sensoren zur Überwachung von Umgebungsbedingungen (z.B. Temperatur, Luftfeuchtigkeit und Beheizung) oder die Entwicklung eines Systems zur Fernsteuerung und Überwachung ausserhalb des Wohnbereiches
Das Smart-Garden-Projekt bietet eine innovative Lösung für die Gemüsezucht zu Hause und zeigt das Potenzial von IoT-Technologie für die Unterstützung von Home-Garden.

Muss-Ziel SOLL-IST

Kosten unter 500 halten
IST: Ja (Kosten liegen bei 201.- CHF)

Ist-Werte Auf einem PC, Laptop oder Smartphone anzeigen lassen
IST: Ja (via Node-Red) Zusätzlich via LCD-Display

Soll von weiteren Sensoren erweiterbar sein
IST: Ja

Automatische Bewässerung
IST: Ja

Winterbeständig
IST: Ja (Hochbeet mit Deckel)

Beleuchtung
IST: Ja (Automatische Beleuchtung)

Wasser Füllstandsanzeige
IST: Ja (Via LCD-Display)


Kann-Ziel Soll-Ist
Speisung über Solar
IST: Nein (Momentan über 5V Speisung)

Öffnung über das Beet über Stellmotoren
IST: Nein (Hat bereits Öffnung, die man manuell auf- und zu machen kann)

Beheizung und Belüftung
IST: Nein und Ja (Für die Belüftung wurde ein Code geschrieben.)

Warnleuchte und Benachrichtigung, wenn der Wasserstand zu tief ist.
IST: Nein (Durch die LCD-Anzeige nicht mehr nötig.)

Persönliche Ziele Soll-Ist
Know-How Programmierung vertiefen
IST: Ja (Noch kein Profi, aber ein bisschen besser geworden.)

Know-How Microcontroller vertiefen
IST: Ja (Noch kein Profi, aber ein bisschen besser geworden.)

Know-How Pflanzenbau
IST: Nein (Das kommt mit der Zeit, wenn das System automatisch läuft.)

Spass haben
IST: Ja (Hat Spass gemacht und war auch frustrierend.)

Pflanzen sollten gedeihen
IST: Ja (Die Radieschen gedeihen.)


Git-Hub Seite: https://github.com/D-ido/Smart-Garden
