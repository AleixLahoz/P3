PAV - P3: detección de pitch
============================

Esta práctica se distribuye a través del repositorio GitHub [Práctica 3](https://github.com/albino-pav/P3).
Siga las instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para realizar un `fork` de la
misma y distribuir copias locales (*clones*) del mismo a los distintos integrantes del grupo de prácticas.

Recuerde realizar el *pull request* al repositorio original una vez completada la práctica.

Ejercicios básicos
------------------

- Complete el código de los ficheros necesarios para realizar la detección de pitch usando el programa
  `get_pitch`.

   * Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.

  ```cpp
  for (unsigned int l = 0; l < r.size(); ++l)
    {
      
      for (unsigned int n = 0; n < (x.size() - l); n++)
      {
            r[l] = r[l] + x[n] * x[n + l];
      }

      r[l] = (1.0F / x.size()) * r[l];

    }
  ```

   * Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un sonido sonoro
     y su periodo de pitch; y, en otro *subplot*, se vea con claridad la autocorrelación de la señal y la
   posición del primer máximo secundario.

   NOTA: es más que probable que tenga que usar Python, Octave/MATLAB u otro programa semejante para
   hacerlo. Se valorará la utilización de la librería matplotlib de Python.

  `Gràfica amb PYTHON`

  <img src="img/python.png" width="720" align="center">

  `Codi utilitzat:`

  ```python
  import numpy as np
  import matplotlib.pyplot as plt
  import scipy.io.wavfile as waves
  import math as math

  #Especificamos el archivo de audio a través de su PATH
  archivo = 'C:/Users/aleix/OneDrive/Escritorio/PAV/P3/adeu.wav'
  #Leemos el archivo de audio, obteniendo la frecuencia de muestreo y el vector con sus muestras
  fmuestreo, audio = waves.read(archivo)

  # Creamos el vector de Tiempo para el eje x
  dt = 1/fmuestreo
  tab = []
  for x in range(0, len(audio)):
      tab.append(x*dt)

  #Usamos ventana de Hamming para enventanar el AUDIO
  a0 = 0.53836
  a1 = 0.46164
  window = []
  for n in range (0, len(audio)):
      window.append(a0 - a1 * math.cos((2 * math.pi  * n) / (len(audio) - 1)))

  #Recalculamos el audio enventanado
  audioEnventanado = audio*window
  #Calculamos la autocorrelacion del audio
  autocorrelacio = np.correlate(audioEnventanado, audioEnventanado, "same")
  #Ajustamos la distancia de la autocorrelacion
  distancia = np.arange(-0.5*len(autocorrelacio), 0.5*len(autocorrelacio), 1)


  # Salida a gràfico
  # Configuramos subplots, espaciados y límites de los ejes, títulos y estilos de línea
  plt.subplot(211)
  plt.title("Senyal d'Audio")
  plt.plot(tab, audio, linewidth =0.7)
  plt.xlim(0,tab[len(audio)-1])
  plt.xlabel('Temps (s)')
  plt.ylabel('WaveForm')
  plt.grid(True)
  plt.subplot(212)
  plt.xlabel('Distancia')
  plt.ylabel('Autocorrelació')
  plt.plot(distancia, autocorrelacio, linewidth =0.7)
  plt.xlim(-0.5*len(autocorrelacio),0.5*len(autocorrelacio))
  plt.xticks(np.arange(-4000, 4001, step=1000))
  plt.grid(True)
  plt.show()
  ```

  `Gràfica amb MATLAB`

  <img src="img/matlab.png" width="720" align="center">

  `Codi utilitzat:`

  ```matlab
  close all;
 

  file_name = 'adeu.wav';
  [senyal, Fs] = audioread(file_name);
 

  w = hamming(length(senyal));
  senyal_finestra = senyal.*w;
  subplot(3,1,1);
  plot(senyal_finestra);
  text(4538, 0.4068, "max 1");
  text(4876, 0.3871, "max 2");
  Fpitch1 = Fs/(4876-4538);
  str1 = strcat("Fpitch = ", num2str(Fpitch1), " Hz");
  t = text(6500,0.3, str1);
  t.Color = 'red';
  t.FontSize = 16;
  ylabel('x[n]');
  xlabel('Sample Number');
  title('SENYAL SONOR','FontSize',14);
 

  %%%%%%%%%%%%%%%%%%%
  % AUTOCORRELACIÓ  %
  %%%%%%%%%%%%%%%%%%%
 

  autocorrelacio = xcorr(senyal_finestra);
  subplot(3,1,2);
  plot(autocorrelacio);
  text(9408, 65.9, "Max secundari = 337", 'FontSize', 13);
  Fpitch2 = Fs/(9408-9071);
  str2 = strcat("Fpitch = ", num2str(Fpitch2), " Hz");
  t = text(13000,50, str2);
  t.Color = 'red';
  t.FontSize = 16;
  title('AUTOCORRELACIÓ','FontSize',14);
  xlabel('Sample Number');
  ylabel('r[n]');
 

  %%%%%%%%%%%%%%%%%%%%
  %MATLAB PITCH APROX%
  %%%%%%%%%%%%%%%%%%%%
 

  [f0, idx] = pitch(senyal, Fs);
  subplot(3,1,3);
  plot(idx, f0);
  ylabel("FPitch [Hz]");
  xlabel("Sample number");
  title("MATLAB PITCH APROXIMATION",'FontSize',14);
  ```



* Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la
     autocorrelación. Inserte a continuación el código correspondiente.

    `Codi utilitzat:`

     ```cpp
     vector<float>::const_iterator iR = r.begin(), iRMax = iR;
     //Creem dos iteradors auxiliars, que pendran els valors immediament inferior i posterior
     //al valor de la posició actual de l'iterador
     vector<float>::const_iterator iRanterior = r.begin();
     vector<float>::const_iterator iRposterior = r.begin();

     //Iniciem iRposterior
     iRposterior = iR + 1;

     /*Bucle que serveix per establir el miním valor coherent del segon pic de l'autocorrelació
      Utilitzem 3 condicions:
      - 1era condició: Ens assegura que arribem fins abaix del primer pic de l'autocorrelació
      - 2na condició: Ens assegura, en cas que el primer pic baixi molt ràpid, que superem el valor
        mínim establert 
      - 3era condició: Ens assegura que en cas de tenir un primer pic molt ample, avançem el suficient
     com per a no pendre com a segon pic un valor que encara sigui del primer pic
     */
     while (*iR > *iRposterior || iR < r.begin() + npitch_min || *iR > 0.0F)
     {
      ++iR;
      ++iRposterior;
     }

     iRMax = iR;

     /* Un cop hem establert el valor mínim coherent de la distancia al segon pic,
     comprovem fent ús de iRanterior i iRposterior que iRMax es trobi en un pic, comprovant
     que iR és a l'hora major que iRanterior i iRposterior
     - Establim com a condició que iR no pot superar el llindar npitch_max - 
     */
     while (iR < r.begin() + npitch_max)
     {
       if (*iR > *iRMax)
     {
       iRanterior = iR - 1;
       iRposterior = iR + 1;
       if (*iR > *iRanterior && *iR > *iRposterior)
       iRMax = iR;
     }
       ++iR;
     }

     unsigned int lag = iRMax - r.begin();
    ```



* Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.

  Després de jugar amb els diferents llindars, el que ens ha resultat més precís ha estat el que
  es mostra en el codi a continuació, on principalment hem establert un total de 4 condicions per a 
  determinar si es tractava d'un VOICED o UNVOICED sound (només s'ha de complir una de les quatre
  per a determinar el so com a unvoiced):

  1) Potència del so inferior a -50.5 dB
  2) R1Normalitzada inferior al llindar mínim de 0.7
  3) RMAXNormalitzada inferior al llindar mínim de 0.3

  4) Finalment, com que jugar amb les 3 condicions anteriors de manera independent no ens permetia
  discriminar amb precisió situacions on els paràmetres entraven en conflicte, vam afegir una
  última condició, combinant els dos paràmetres R1Norm i RMaxNorm, establint que si aquests dos
  es troben alhora per sota dels llindars establerts (menys restrictius que a les condicions 2. i 3.)
  decretem el segment com UNVOICED. D'aquesta manera podem corregir els punts on un dels dos paràmetres
  pren un valor erroni o incoherent, i compensem així aquest tipus d'errors.

`Codi utilitzat:`

```cpp
if (pot < -50.5 || r1norm < 0.70 || rmaxnorm < 0.3 || (r1norm < 0.935 && rmaxnorm < 0.4))
  {
    return true; //Unvoiced Sound
  }
  else
  {
    return false; //Voiced Sound
  }
```

- Una vez completados los puntos anteriores, dispondrá de una primera versión del detector de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
  
    - Inserte una gráfica con la detección de pitch incorporada a `wavesurfer` y, junto a ella, los 
      principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
    (r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
    autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).

    Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.

      Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que
    en esta práctica es de 15 ms.


  <img src="img/wave1.png" width="720" align="center">

  
* Observant els diferents paràmetres, podem extreure les conclusions següents:
   + Tal i com vam observar a la pràctica anterior, el paràmetre de la potència [dB] és un paràmetre que
    resulta útil si es combina amb d'altres paràmetres, ja que sinó, com podem veure, en segons quines
          parts de l'audio, aquesta no pateix canvis suficientment signficiatius com per utilitzar-la com a 
    criteri únic. Per tant, l'hem d'utilitzar establint un llindar no gaire restrictiu, com podria ser
    el de considerar Unvoiced als segments amb potència inferior als -70dB.
   + Pel que fa als paràmetres referents a l'autocorrelació, observem que presenten contorns similars
    a la gràfica del pitch, pero amb algunes irregularitats (sobretot Rmax normalitzada), per tant, 
    tal i com també hem comentat amb la potència, el més addient seria establir criteris que tinguin
    en compte els tres paràmetres a l'hora. Observem com el contorn de la gràfica de R1Norm s'assimila 
          bastant a la del pitch, aleshores, per corregir els seus errors, per exemple, podriem considerar Unvoiced 
          qualsevol trama que tingui o bé RmaxNorm o R1Norm per sota d'un cert llindar, ja que així evitem les 
          irregularitats d'ambudes gràfiques. A simple vista és difícil establir els llindars, però podriem dir
    que tots dos han de ser superiors aproximadament a 0.6 per a dir que estem en estat VOICED.
    

* Use el detector de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
      su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
    ilustrativa del resultado de ambos detectores.

  <img src="img/wave2.png" width="720" align="center">

  Comparant el resultat del nostre sistema amb la detecció del Wavesurfer veiem que obtenim resultats que, en la seva gran 
  majoria, guarden una gran semblança. Les principals diferències que podem observar entre la nostra detecció respecte la 
  del Wavesurfer són certs certs pics, on el nostre sistema detecta un tram VOICED quan realment no ho és. Aquests pics 
  representen un error que pot ser fàcilment correjit ajustant els paràmetres en detecció i, sobretot, aplicant un filtre 
  de mediana en l'etapa de postprocessament, tal com veurem en els següents apartats.
  De tota manera, arribem a la conclusió que el nostre sistema funciona correctament, tot i que encara el podem millorar.

```
Observem que a partir d'un petit ajustament en els paràmetres, ja obtenim un nivell de detecció de 
de pitch molt elevat (87% d'accuracy aproximadament). 
```
  
  * Optimice los parámetros de su sistema de detección de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
  `pitch_db/train`..

<img src="img/score.png" width="640" align="center">

   * Inserte una gráfica en la que se vea con claridad el resultado de su detector de pitch junto al del
     detector de Wavesurfer. Aunque puede usarse Wavesurfer para obtener la representación, se valorará
   el uso de alternativas de mayor calidad (particularmente Python).

`MOSTRA 1:`

<img src="img/results1.png" width="640" align="center">

`MOSTRA 2:`

<img src="img/results2.png" width="640" align="center">

Observem que obtenim resultats molt similars als del detector del WaveSurfer, fins i tot millors en l'àmbit de detecció
de segments VOICED i UNVOICED. A més, la nostra gràfica és més suau gràcies al filtre de mediana aplicat en el postprocessat.
   

Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  detector a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del detector. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la detección de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.

`MENSAJE DE AYUDA:`

<img src="img/docopt.png" width="640" align="center">

`SCRIPT MODIFICADO:`

<img src="img/script.png" width="640" align="center">

- Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de detección
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, *center clipping*, etc.
  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el detector, en concreto, de los que
    gobiernan la decisión sonoro/sordo.
  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

  Encontrará más información acerca de estas técnicas en las [Transparencias del Curso] y 
  en [Spoken Language Processing](https://discovery.upc.edu/iii/encore/record/C__Rb1233593?lang=cat).
  También encontrará más información en los anexos del enunciado de esta práctica.

  Incluya, a continuación, una explicación de las técnicas incorporadas al detector. Se valorará la
  inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.

  También se valorará la realización de un estudio de los parámetros involucrados. Por ejemplo, si se opta
  por implementar el filtro de mediana, se valorará el análisis de los resultados obtenidos en función de
  la longitud del filtro.


### PREPROCESADO:  *CENTER CLIPPING*

Per a millorar el nostre senyal de waveform aplicarem una transformació no lineal anomenada Center Clipping, que consisteix 
en posar a 0 els valors de la senyal que no superin un cert threshold. D'aquesta manera aconseguim eliminar la part de la 
senyal que no ens interessa i quedar-nos amb la part útil, és a dir, la part que està per sobre d'aquest límit. Fent 
recerca hem vist que el valor òptim del límit és el 30% del valor màxim de la senyal, de tota manera, en el nostre cas 
obtenim un millor resultat si utilitzem com a límit el 80% de la potència de la senyal.
El fet d'afegir el center clipping ha suposat una millora d'un 1% en el percentatge total d'evaluació utilitzant els fitxer
de la base de dades.


<img src="img/clipping.png" width="720" align="center">

```
En aquest plot de Matlab podem veure l'efecte del center clipping en una senyal sonora. És evident que eliminem part de la
senyal que no ens és útil, però mantenim les parts que ens són més útils, és a dir, els màxims.
```
`Codi utilitzat:`

```cpp
for (unsigned int i = 0; i < x.size(); i++)//Càlcul del llindar
  {
    potencia += abs(x[i]) * abs(x[i]);
  }
  potencia = (1.0F / x.size()) * potencia;
  
  CL = 0.8 * potencia;

  //  Apliquem el Center Clipping
  for (unsigned int i = 0; i < x.size(); i++)
  {
    if (x[i] >= CL)
    {
      x[i] -= CL;
    }
    else if (abs(x[i]) < CL)
    {
      x[i] = 0;
    }
    else if (x[i] <= -CL)
    {
      x[i] += CL;
    }
  }
```

### POSTPROCESADO: *FILTRO DE MEDIANA*

En la part de postprocessat apliquem la tècinca del filtre de mediana. Aquest és un filtre de funcionament senzill, 
però és alhora efectiu. Consisteix en recórrer cada valor del vector on emmagatzemem les freqüències de pitch, i 
reemplaçar-lo per el valor de la mediana dels valors del voltant. Aquest filtre ens ajudarà a eliminar certs errors 
puntuals, com ara els pics que ja hem mencionat en un dels apartats anteriors. De tota manera, aquest filtre no ajuda a 
millorar quantitativament la puntuació total, tot i que ens soluciona errors significatius de la senyal.

Considerant quina és la mida que hauria de tenir la finestra nosaltres hem probat els dos valors que ens han semblat més 
adients, aquests són 3 i 5. En la imatge següent podem veure els resultats obtinguts utilitzant cada una de les finestres. 
Podem concloure que obtenim millors resultats si utilitzem una finestra de 3 espais, ja que aconseguim corregir certs 
errors de la senyal sense introduir una distorsió notòria, cosa que sí que passa amb la finestra de 5 posicions. Per últim, 
cal mencionar que el filtre no s'aplica en els extrems del vector de freqüències ja que són valors complicats d'analitzar i 
que tampoc tenen rellevància en el resultat final.

<img src="img/mediana.png" width="720" align="center">

`Codi utilitzat:`

```cpp
  unsigned int mida_finestra = 3;
  float finestra[mida_finestra];
  float aux_ord;

  for (unsigned int i = 1; i < f0.size() - 1; ++i)
  { //recorrem els elements respectant els limits de la finestra
    for (unsigned int p = 0; p < mida_finestra; ++p)
    { //guardem els elements de la finestra (-1 0 1 )
      finestra[p] = f0[i - 1 + p];
    }
    for (unsigned int p = 0; p < mida_finestra; ++p)
    { //algoritme d'ordenacio
      int min = p;
      for (int k = p + 1; k < mida_finestra; k++)
      {
        if (finestra[min] > finestra[k])
        {
          min = k;
        }
      }
      aux_ord = finestra[p];
      finestra[p] = finestra[min];
      finestra[min] = aux_ord;
    }

    f0[i] = finestra[1]; //la mostra que ha quedat al mig serà la bona
    //apliquem el filtre a partir de la mostra 1 fins N-1 per evitar problemes amb marges
  }
```

Evaluación *ciega* del detector
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
detector con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.