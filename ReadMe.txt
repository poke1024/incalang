Quick Guide To Inca
===================

Willkommen bei Inca.

1.1 Installation

Inca ben�tigt DirectX 8. Bitte benutzen Sie das im
Ordner "Release" der beiliegenden CD liegende Programm
DX80NTger.exe zur Installation von DirectX 8, falls
auf Ihrem Rechner DirectX 8 noch nicht installiert
sein sollte.

1.2 Lizenz

Inca, der Quelltext dazu, die Binaries, die Beispiel-
programme, sowie das Manual, sind unter der Gnu Public
License (GPL) ver�ffentlicht, soweit dies bei einzelnen
Dateien nicht die Rechte anderer Autoren verletzt. Die
genauen Lizenzbestimmungen entnehmen Sie bitte der Datei
"COPYING.TXT" im Ordner "Development".

2.1 Aufbau

Im Ordner "Release" befindet sich eine lauff�hige
Distribution von Inca, mit der man Programme schreiben
und laufen lassen kann. Das Verzeichnis "Development"
enth�lt die beiden Visual C Projekte, den kompletten
Quelltext zu Inca sowie andere Entwicklungsunterlagen.

Das Verzeichnis Release enth�lt vier Objekte:
	
	Inca.exe
		Die Entwicklungsumgebung mit Compiler
		und allem anderen - einfach starten

	Data
		Ein Verzeichnis mit wichtigen Dateien
		f�r Inca - muss im selben Verzeichnis
		liegen wie Inca.exe

	manual.pdf
		Ein nicht ganz aktuelles Manual der
		Funktionen und F�higkeiten von Inca

	Samples
		Eine Reihe Beispielprogramme f�r Inca

2.2. Starten des ersten Programmes

Nach dem Starten von Inca.exe erscheint ein Textfenster,
in dem neue Programme geschrieben werden k�nnen. Man kann
aber auch einfach mittels Datei->�ffnen eines der Beispiele
�ffnen, z.B. Raytracer/Raytracer.Inca. Nach dem �ffnen,
erscheint ein neues Fenster mit dem Quellcode des Beispiel-
programms. Starten kann man dieses Programm durch Dr�cken
des Pfeiles in der Toolbar (rechts neben der Diskette).
Auf manchen Systemen passiert beim ersten Mal starten
leider gar nichts, dann sollte man einfach auf Stop
(Quadrat) dr�cken und dann nochmal den Pfeil.

2.3. Der Editor

Der Editor formattiert eigenst�ndig den Quellcode.

Geschweifte Klammern wie in

	if( x ) {
		y = 2;
		z = 3;
	}

kann man zu

	if( x ) { ... }

komprimieren, indem man den Textcursor auf die Zeile mit der
sich �ffnenden Klammer setzt und Shift-Tabulator dr�ckt. Ebenso
dekomprimiert man sie wieder.

Die Programme werden als unformattierte Textdateien gespeichert.

2.4. Die Beispielprogramme

Hier ist eine kurze Liste, der beiliegenden Beispielprogramme
im Verzeichnis Samples, und was sie tun:

	Bounce
		OpenGL Beispiel einer sich deformierenden
		h�pfenden Kugel

	ERModeller
		Ein Modellierer f�r Datenbank ER Diagramme,
		den ich f�r ein Praktikum geschrieben habe.
		Um ein Beispiel zu sehen, starte man das
		Programm, �ffne mit dem aus der links in der
		Leiste erscheinenden Button "Load" die Datei
		"ERModelSample" im gleichen Verzeichnis und
		erhalte dann die Vollansicht. Indem man die
		Maus an die R�nder des Fensters bewegt, scrollt
		man, die Vollansicht erh�lt man zur�ck durch
		den Knopf "Center" links. Mit dem Mausrad kann
		man rein- und rauszoomen (eines meiner Lieblings-
		features). Die anderen Funktionen sollten
		selbsterkl�rend sein.

	GraphEditor
		Ein einfacher Editor f�r gerichtete Graphen

	Graphics
		Ein Verzeichnis mit verschiedenem Beispielcode
		f�r Grafiksachen, um zum Beispiel ein .bmp Bild
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
		Programm sieht man, wie man gr��ere Programme mittels
		des "module" Konstrukts sinnvoll strukturieren kann

	Water
		OpenGL Beispiel einer Wasseroberfl�che. Mit den Pfeil-
		tasten dreht man die Ansicht, mittels Space l��t man
		Tropfen hineinfallen

	SimpleThreads
		Ein Beispiel von Incas eingebauter Thread Library.
		Das besondere hier ist, da� nicht Windows Threads
		benutzt werden, sondern ein eigener Scheduler, weshalb
		Inca mit seinen Threads bei Bedarf soft real time
		requirements im Mikrosekundenbereich erf�llen kann

	StreamSample
		Zeigt, wie man Streams in Inca benutzt, wie man Files
		schreibt, und au�erdem, wie man mit den zlib Streams
		einen String komprimiert und dekomprimiert	

	Language
		Verschiedene Sample Codes, die Sprachfeatures zeigen,
		wie Referenzen, Funktionstemplates und Klassentemplates

	Lenstra
		Sample Code, der Incas BigInt Library benutzt, um
		Lenstras Elliptic Curves Algorithmus zur Faktorisierung
		gro�er Zahlen zu implemeniteren

	TailCall
		Ein kurzes Beispiel, das demonstriert, wie Inca fuer
		gewisse Funktionsaufrufe sogenannte Tail-Call-Optimierungen
		vornimmt, mit denen beliebig hohe Rekursionstiefen erreicht
		werden koennen, ohne einen Stackueberlauf zu erzeugen

2.5. Die Sprache

Inca hat eine Reihe komplexer Sprachfeatures, die hier aufzuz�hlen
den Rahmen sprengen w�rde. Dazu geh�ren unter anderem Mehrfachvererbung,
Operator Overloading, Funktions- und Klassentemplates, virtuelle
Funktionen, Referenzen und Zeiger, komplexe Datentypen (BigInt, color,
point, vector), namespaces, und einiges mehr. Es gibt relativ wenige
Konstrukte in C++ oder Java, die in Inca nicht vorhanden sind.

Der Compiler ist wie Java ein Look Ahead Compiler, d.h. anders als z.B.
in C++ k�nnen Klassen und Typen benutzt werden, bevor sie im Quelltext
deklariert wurden, wie z.B. hier:

	void f() {
		A myObject;
		myObject.doSomething();
	}

	class myObject {
		void doSomething() { ... }
	}

Dies ist insbesondere erw�hnenswert, als Inca, anders als Java,
stackgebundene Objekte (nicht nur Referenzen hiervon) unterst�tzt.
Im obigen Beispiel wird myObject tats�chlich auf dem Stack
konstruiert und wieder abgebaut.

Inca kommt mit einer Reihe vielseitiger Bibliotheksroutinen, so zum
Beispiel einer eigenen Threadbibliothek, einer OpenGL Anbindung,
Funktionen zur Ausgabe und Bearbeitung, dem Speichern und dem Laden
von 2d Grafiken, Streams, mathematische Funktionen, Dateifunktionen,
Zahlentheoriefunktionen und einigem mehr.

�ber die wichtigsten der Funktionen in diesen Bibliotheken sowie einige
der F�higkeiten und Eigenheiten der Inca-Sprache geben manual.pdf
sowie einige der Beispielprogramme Auskunft.

2.6. Der Debugger

Inca verf�gt �ber einen Debugger, der bei Fehlern (z.B. bus errors)
automatisch anspringt und an der entsprechenden Stelle im Code mit
einer Anzeige des Call Stacks der Funktionen und der Variablenwerte
anh�lt. Man kann den Debugger auf manuell mittels der Funktion
debugger() aufrufen, oder auch mit dem fatal() Kommando, das als
Parameter einen String nimmt. Weiterhin besteht die M�glichkeit durch
Klicken in den linken Rand des Quelltextfensters (links von der schwarzen
senkrechten Linie) Breakpoints zu setzen (sie werden als rote Kreise
dargestellt) - an diesen Punkten wird die Programmausf�hrung dann
unterbrochen.

3.1. F�r Entwickler

Inca besteht aus zwei Programmen, n�mlich (1) dem Editor,
Compiler und Debugger in Inca.Exe, und (2) dem Interpreter
des virtuellen Maschinencodes in IncaVM.Exe. Entsprechend
sind diese beiden Programme auch unterschiedliche VC
Projekte. 

Der Source Code ist sehr ungepflegt, da er zun�chst in einem
eigenen Windows Framework geschrieben wurde, dann auf Mac OS X
portiert wurde, um schlie�lich in MFC f�r Windows portiert zu
werden. Daher sind speziell f�r das GUI etliche Zwischenschichten
eingezogen, die etwas verwirrend sind.

Der Compiler selbst hat mehrere Stellen, an denen �ber die
Zeit der Code immer unsauberer und unklarer wurde. In einigen
Passagen und Modulen ist der Code furchtbar undurchdacht und
m��te eigentlich neu geschrieben werden. In sehr gro�en Teilen
halte ich das Projekt trotzdem f�r relativ sinnvoll und sauber
angelegt.

3.2. Motivation

Inca wurde urspr�nglich geschrieben, aus dem Wunsch, eine einfache,
unkompliziert zu bedienende, kostenlose, und vielf�ltig einsetzbare
Entwicklungsumgebung zu schreiben. Ich w�rde mir w�nschen, da� viele
Menschen es einsetzen, egal ob als Entwicklungsumgebung, oder als
Testplattform f�r Erweiterungen oder Modifikationen von Programmier-
sprachen.

Bernhard Liebl
Bernhard.Liebl@gmx.org
