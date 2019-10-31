#!/bin/bash

echo "500 750 1000 1250 1500 1750 2000 3000 4000 5000" > peso-arq;
#echo "100 250" > peso-arq;
echo "1 2 3 4 6 8 10 12 16" > nroproc-arq;
file="amd-hp.dad";
#file="pp-dados/grenoble/genepi.dad";

awk '{print $2" "$2" "$3" "$6}' $file > /tmp/tmp01
for peso in `cat peso-arq` ;
  do 
    `rm -f $file.$peso`;
    `cat /tmp/tmp01 | grep " $peso" > /tmp/dados-$peso`;
    while read line;
       do
         var=`echo $line | awk '{print $2}'`;
         if [ "$var" == 1 ] ; then
            ts=`echo $line | awk '{print $4}'`;
            #echo "ts= "$ts;
         else 
            tp=`echo $line | awk '{print $4}'`;
            speedup=`echo $ts" / "$tp | bc -l`;
            echo "speedup("$peso")= "$ts"/"$tp"= "$speedup;
            echo $line" "$speedup >> $file.$peso
         fi
       done < "/tmp/dados-$peso";
  done

echo "ATENCAO aos parametros:";
echo " - arquivo de dados: "$file;
echo " - tamanhos da matriz: "`cat peso-arq`
echo " - nro de threads: "`cat nroproc-arq`
echo "Caso o resultado nao tenha aparecido na tela, verifique os parametros!"
echo "***********"
echo "Proximos passos:"
echo " - Altere o arquivo <nome>.gnuplot"
echo " - Execute: gnuplot <nome>.gnuplot"
`rm -f peso-arq nroproc-arq`;

