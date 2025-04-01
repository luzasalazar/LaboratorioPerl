#!/usr/bin/perl 

#**************************************************************
#                       Pontificia Universidad Javeriana
#     Materia: Sistemas Operativos
#     Fichero: Automatización para ejecuciones por lotes
#     Autor: Adriana Salazar
#     Descripcion: Fichero para la ejecución de ejecutables
#****************************************************************/

#Guarda lo que se obtiene al ejecutar el comando pwd (directorio en que esta el archivo)
$Path = `pwd`; 

#Elimina el salto de linea que se guarda al final de la cadena en $Path
chomp($Path);

#Crea un arreglo que contiene el nombre del ejecutable que se queiere ejecutar
@Nombre_Ejecutable = ("mm_clasico");

#Crea un arreglo con los tamaños de las matrices que se quiere para los casos de prueba
@Size_Matriz = (500, 1000);

#Crea un arreglo con la cantidad de hilos entre los que se va a divir el trabajo
@Num_Hilos = (1,2);

#Define el numero de veces en que se ejecutara cada caso de prueba
$Repeticiones = 30;

foreach $Nombre_Ejecutable (@Nombre_Ejecutable) { #Ciclo para cada ejecutable guardado en @Nombre_Ejecutable
    foreach $size (@Size_Matriz) { #Ciclo para cada tamaño de matriz guardado en @Size_Matriz
        foreach $hilo (@Num_Hilos) { #Ciclo para cada número de hilos guardado en @Num_Hilos

            $file = "$Path/$Nombre_Ejecutable-".$size."-Hilos-".$hilo.".dat"; #Nombre del archivo que guarda los resultados

            for ($i = 0; $i < $Repeticiones; $i++) { #Ciclo para ejecutar el programa n repeticiones

               system("$Path/$Nombre_Ejecutable $size $hilo  >> $file"); #Ejecuta el programa mm_clasico con cada size e hilo

               # printf("$Path/$Nombre_Ejecutable $size $hilo \n");
            }

            close($file); #Cierra el archivo

            $p = $p + 1;
        }
    }
}
