Quick Guide To Inca
===================

Willkommen bei Inca.

1.1 Installation

Inca benötigt DirectX 8. Bitte benutzen Sie das im
Ordner "Release" der beiliegenden CD liegende Programm
DX80NTger.exe zur Installation von DirectX 8, falls
auf Ihrem Rechner DirectX 8 noch nicht installiert
sein sollte.

1.2 Lizenz

Inca, der Quelltext dazu, die Binaries, die Beispiel-
programme, sowie das Manual, sind unter der Gnu Public
License (GPL) veröffentlicht, soweit dies bei einzelnen
Dateien nicht die Rechte anderer Autoren verletzt. Die
genauen Lizenzbestimmungen entnehmen Sie bitte der Datei
"COPYING.TXT" im Ordner "Development".

2.1 Aufbau

Im Ordner "Release" befindet sich eine lauffähige
Distribution von Inca, mit der man Programme schreiben
und laufen lassen kann. Das Verzeichnis "Development"
enthält die beiden Visual C Projekte, den kompletten
Quelltext zu Inca sowie andere Entwicklungsunterlagen.

Das Verzeichnis Release enthält vier Objekte:
	
	Inca.exe
		Die Entwicklungsumgebung mit Compiler
		und allem anderen - einfach starten

	Data
		Ein Verzeichnis mit wichtigen Dateien
		für Inca - muss im selben Verzeichnis
		liegen wie Inca.exe

	manual.pdf
		Ein nicht ganz aktuelles Manual der
		Funktionen und Fähigkeiten von Inca

	Samples
		Eine Reihe Beispielprogramme für Inca

2.2. Starten des ersten Programmes

Nach dem Starten von Inca.exe erscheint ein Textfenster,
in dem neue Programme geschrieben werden können. Man kann
aber auch einfach mittels Datei->Öffnen eines der Beispiele
öffnen, z.B. Raytracer/Raytracer.Inca. Nach dem Öffnen,
erscheint ein neues Fenster mit dem Quellcode des Beispiel-
programms. Starten kann man dieses Programm durch Drücken
des Pfeiles in der Toolbar (rechts neben der Diskette).
Auf manchen Systemen passiert beim ersten Mal starten
leider gar nichts, dann sollte man einfach auf Stop
(Quadrat) drücken und dann nochmal den Pfeil.

2.3. Der Editor

Der Editor formattiert eigenständig den Quellcode.

Geschweifte Klammern wie in

	if( x ) {
		y = 2;
		z = 3;
	}

kann man zu

	if( x ) { ... }

komprimieren, indem man den Textcursor auf die Zeile mit der
sich öffnenden Klammer setzt und Shift-Tabulator drückt. Ebenso
dekomprimiert man sie wieder.

Die Programme werden als unformattierte Textdateien gespeichert.

2.4. Die Beispielprogramme

Hier ist eine kurze Liste, der beiliegenden Beispielprogramme
im Verzeichnis Samples, und was sie tun:

	Bounce
		OpenGL Beispiel einer sich deformierenden
		hüpfenden Kugel

	ERModeller
		Ein Modellierer für Datenbank ER Diagramme,
		den ich für ein Praktikum geschrieben habe.
		Um ein Beispiel zu sehen, starte man das
		Programm, öffne mit dem aus der links in der
		Leiste erscheinenden Button "Load" die Datei
		"ERModelSample" im gleichen Verzeichnis und
		erhalte dann die Vollansicht. Indem man die
		Maus an die Ränder des Fensters bewegt, scrollt
		man, die Vollansicht erhält man zurück durch
		den Knopf "Center" links. Mit dem Mausrad kann
		man rein- und rauszoomen (eines meiner Lieblings-
		features). Die anderen Funktionen sollten
		selbsterklärend sein.

	GraphEditor
		Ein einfacher Editor für gerichtete Graphen

	Graphics
		Ein Verzeichnis mit verschiedenem Beispielcode
		für Grafiksachen, um zum Beispiel ein .bmp Bild
		aus einer offscreen gemalten pixmal zu speichern
		(SavePicture.Inca), oder um verschiedene 2d
		Grafikconsolen zu verwenden und deren APIs.

	Includes
		Zeigt wie man in Inca andere Dateien inkludiert

	Level
		OpenGL Beispiel eines Ego Shooters - bewegen kann
		man sich mit den Pfeiltasten auf der Tastatur

	Misc
		Wieder verschiedene Beispiele zu Bibliotheksfunktionen

	Raytracer
		Ein kompletter kleiner Raytracer in Inca, der eine
		Erdkugel rendered, wobei er den earth shader von
		Ken Musgrave implementiert - der ebenfalls von Musgrave
		stammende venus shader ist auskommentiert - an diesem
		Programm sieht man, wie man größere Programme mittels
		des "module" Konstrukts sinnvoll strukturieren kann

	Water
		OpenGL Beispiel einer Wasseroberfläche. Mit den Pfeil-
		tasten dreht man die Ansicht, mittels Space läßt man
		Tropfen hineinfallen

	SimpleThreads
		Ein Beispiel von Incas eingebauter Thread Library.
		Das besondere hier ist, daß nicht Windows Threads
		benutzt werden, sondern ein eigener Scheduler, weshalb
		Inca mit seinen Threads bei Bedarf soft real time
		requirements im Mikrosekundenbereich erfüllen kann

	StreamSample
		Zeigt, wie man Streams in Inca benutzt, wie man Files
		schreibt, und außerdem, wie man mit den zlib Streams
		einen String komprimiert und dekomprimiert	

	Language
		Verschiedene Sample Codes, die Sprachfeatures zeigen,
		wie Referenzen, Funktionstemplates und Klassentemplates

	Lenstra
		Sample Code, der Incas BigInt Library benutzt, um
		Lenstras Elliptic Curves Algorithmus zur Faktorisierung
		großer Zahlen zu implemeniteren

	TailCall
		Ein kurzes Beispiel, das demonstriert, wie Inca fuer
		gewisse Funktionsaufrufe sogenannte Tail-Call-Optimierungen
		vornimmt, mit denen beliebig hohe Rekursionstiefen erreicht
		werden koennen, ohne einen Stackueberlauf zu erzeugen

2.5. Die Sprache

Inca hat eine Reihe komplexer Sprachfeatures, die hier aufzuzählen
den Rahmen sprengen würde. Dazu gehören unter anderem Mehrfachvererbung,
Operator Overloading, Funktions- und Klassentemplates, virtuelle
Funktionen, Referenzen und Zeiger, komplexe Datentypen (BigInt, color,
point, vector), namespaces, und einiges mehr. Es gibt relativ wenige
Konstrukte in C++ oder Java, die in Inca nicht vorhanden sind.

Der Compiler ist wie Java ein Look Ahead Compiler, d.h. anders als z.B.
in C++ können Klassen und Typen benutzt werden, bevor sie im Quelltext
deklariert wurden, wie z.B. hier:

	void f() {
		A myObject;
		myObject.doSomething();
	}

	class myObject {
		void doSomething() { ... }
	}

Dies ist insbesondere erwähnenswert, als Inca, anders als Java,
stackgebundene Objekte (nicht nur Referenzen hiervon) unterstützt.
Im obigen Beispiel wird myObject tatsächlich auf dem Stack
konstruiert und wieder abgebaut.

Inca kommt mit einer Reihe vielseitiger Bibliotheksroutinen, so zum
Beispiel einer eigenen Threadbibliothek, einer OpenGL Anbindung,
Funktionen zur Ausgabe und Bearbeitung, dem Speichern und dem Laden
von 2d Grafiken, Streams, mathematische Funktionen, Dateifunktionen,
Zahlentheoriefunktionen und einigem mehr.

Über die wichtigsten der Funktionen in diesen Bibliotheken sowie einige
der Fähigkeiten und Eigenheiten der Inca-Sprache geben manual.pdf
sowie einige der Beispielprogramme Auskunft.

2.6. Der Debugger

Inca verfügt über einen Debugger, der bei Fehlern (z.B. bus errors)
automatisch anspringt und an der entsprechenden Stelle im Code mit
einer Anzeige des Call Stacks der Funktionen und der Variablenwerte
anhält. Man kann den Debugger auf manuell mittels der Funktion
debugger() aufrufen, oder auch mit dem fatal() Kommando, das als
Parameter einen String nimmt. Weiterhin besteht die Möglichkeit durch
Klicken in den linken Rand des Quelltextfensters (links von der schwarzen
senkrechten Linie) Breakpoints zu setzen (sie werden als rote Kreise
dargestellt) - an diesen Punkten wird die Programmausführung dann
unterbrochen.

3.1. Für Entwickler

Inca besteht aus zwei Programmen, nämlich (1) dem Editor,
Compiler und Debugger in Inca.Exe, und (2) dem Interpreter
des virtuellen Maschinencodes in IncaVM.Exe. Entsprechend
sind diese beiden Programme auch unterschiedliche VC
Projekte. 

Der Source Code ist sehr ungepflegt, da er zunächst in einem
eigenen Windows Framework geschrieben wurde, dann auf Mac OS X
portiert wurde, um schließlich in MFC für Windows portiert zu
werden. Daher sind speziell für das GUI etliche Zwischenschichten
eingezogen, die etwas verwirrend sind.

Der Compiler selbst hat mehrere Stellen, an denen über die
Zeit der Code immer unsauberer und unklarer wurde. In einigen
Passagen und Modulen ist der Code furchtbar undurchdacht und
müßte eigentlich neu geschrieben werden. In sehr großen Teilen
halte ich das Projekt trotzdem für relativ sinnvoll und sauber
angelegt.

3.2. Motivation

Inca wurde ursprünglich geschrieben, aus dem Wunsch, eine einfache,
unkompliziert zu bedienende, kostenlose, und vielfältig einsetzbare
Entwicklungsumgebung zu schreiben. Ich würde mir wünschen, daß viele
Menschen es einsetzen, egal ob als Entwicklungsumgebung, oder als
Testplattform für Erweiterungen oder Modifikationen von Programmier-
sprachen.

Bernhard Liebl
Bernhard.Liebl@gmx.org
