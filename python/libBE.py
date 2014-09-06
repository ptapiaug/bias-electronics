#General
import numpy as np
import os, sys, shutil
import time
import pprint # Pretty print for results
import ipdb # ipdb.set_trace() #for debuggin

#Pls and statistics functions.
import pls as plsLib
import metrics 
from scipy import stats,e

#Plot libs
from matplotlib.pylab import transpose
import matplotlib.pyplot as plt
import matplotlib.ticker as tck

####Funciones generales####

def getData(path):
#in: string
#out: list of lists
#fn: path of objective folder. Read every file and put the info into a virtual list of list. 
    
    text=open(path,'r')
    row1=[]
    row2=[] 
    for line in text:
        data=line.split()
        row1.append(float(data[0]))
        row2.append(float(data[1]))
    return row1,row2

def getDataMult(path):
#in: string
#out: list of lists
#fn: path of objective folder. Read every file and put the info into a virtual list of list. 
    
    X=[]
    Y=[]
    text=open(path,'r')
    
    row1=[]
    row2=[] 
    for line in text:
        data=line.split()
        row1.append(float(data[0]))
        row2.append(float(data[1]))

    for i in range(100):
        X.append(row1[4096*i:4096*(i+1)])
        Y.append(row2[4096*i:4096*(i+1)])
    
    return X,Y

def vNominal(minV, maxV, paso, tScan, BINS):

    mixV=abs(maxV-minV)
    dV=mixV/(BINS/2)
    dt=tScan/BINS

    x=[i for i in range(BINS)]
    xR=[] # eje x con tiempo real

    for i in range(BINS*paso):
        xR.append(dt*i/paso)

    y1=[]
    y2=[]

    for i in range(BINS/2):
        for j in range(paso):
            y1.append(-mixV/2+dV*x[i])
            y2.append(mixV/2-dV*x[i])

    y=y1+y2

    return xR,y



def vActual(y,k,paso,tScan,BINS):
    
    x=[y[0] for i in range(paso)]
    aux=y[0]
    dt=tScan/BINS
    for i in range(BINS-1):
        x_i=[]
        
        for j in range(paso):
            t=(j+1)*dt/paso
            x_i.append(y[(i+1)*paso]+(aux-y[(i+1)*paso])*e**(-t/k))
        
        aux=x_i[paso-1]
        x=x+x_i

    return x


def current(v,R,vSC):

    y=[]
    # ipdb.set_trace()
    for i in range(len(v)):
        if(abs(v[i])<=vSC):
            y.append(0)
        else:
            y.append(v[i]/R)

    return y

def getCurves(minV, maxV, paso, tScan, BINS, k, R, vSC):

    (x,vNom)=vNominal(minV, maxV, paso, tScan, BINS)
    vAct=vActual(vNom,k,paso,tScan,BINS)

    iNom=current(vNom,R,vSC)
    iAct=current(vAct,R,vSC)

    return (x,vNom,vAct,iNom,iAct)

def error(nom,actual,paso,BINS):
    error=[]
    average=[]
    for j in range(BINS/2):
        i1=int(j*paso+paso/2)
        i2=(BINS)*paso-1-i1
        # ipdb.set_trace()
        error.append(nom[i1]-(actual[i1]+actual[i2])/2)
        average.append((actual[i1]+actual[i2])/2)        
    return error,average


def getVar(Matrix):
    var=[]
    std=[]

    for i in range(len(Matrix[0])):
        row = []
        for j in range(len(Matrix)):
            row.append(Matrix[j][i])
        var.append(np.var(row))
        std.append(np.std(row))

    return var,std
 
def getRms(Matrix):
    rms=[]
    for i in range(len(Matrix[0])):
        row = []
        for j in range(len(Matrix)):
            row.append(Matrix[j][i])
        rms.append(np.var(row)**0.5)
        
    return rms

def getStd(Matrix):
    std=[]
    for i in range(len(Matrix[0])):
        row = []
        for j in range(len(Matrix)):
            row.append(Matrix[j][i])
        std.append(np.std(row))
        
    return std


def delMatlab(sourcePath):
#in: string
#out: none
#fn: inside folder 'source' delete every file with subfix '.m' ('.mat' is included) 
    # folders=os.listdir(sourcePath) #List with names of files
    # folders.sort()
    # for folder in folders:
    #     ipdb.set_trace()
    files=os.listdir(sourcePath)
    for line in files:
        try:
            line.index('.m')
            filePath=sourcePath+'/'+line
            os.remove(filePath)
        except:
            continue
 
def moveData(sourcePath,destinationPath):
#in: string, string
#out: none
#fn: move files (who pass the condition) from 'source' to 'destination' folder
    
    files=os.listdir(sourcePath)
    files.sort()
    if not os.path.exists(destinationPath):
        os.makedirs(destinationPath)
    for i in range(len(files)):
        if(True): #Put here the condition to move or not move the file to destination folder
            shutil.move(sourcePath+'/'+files[i],destinationPath)
        else:
            continue
       
def is_letter(s):
#in: char
#out: boolean
#fn: return True if the char is not a number
    try:
        float(s)
        return False
    except ValueError:
        return True

def getMatrix(path):
#in: string
#out: list of lists
#fn: path of objective folder. Read every file and put the info into a virtual list of list. 
    
    files=os.listdir(path)
    files.sort()
    matrix=[]
    
    for name in files:
        text=open(path+'/'+name,'r')
        row=[]
        for line in text:
            if(is_letter(line[0])):
                continue
            
            line=line.replace('\t',' ') #tab for space
            line=line.replace(',','.')
            line=line[:len(line)-2] # to delete /n
            data=line.split(' ')
            row.append(float(data[1]))
        matrix.append(row)
    
    return matrix
    
def getVector(path):
#in: string
#out: list
#fn: path of the objective file. Put the values on a virtual list.
    Y=[]
    file=open(path,'r')
    for linea in file:
        Y.append(float(linea))    
    
    return Y

def avgMatrix(Matrix):
#in: list of lists OR list
#out: list of Lists
#fn: reduce the lenght of the matrix to the half by taking the avg between the elements Matrix_n and Matrix_n+1
    avgMatrix=[]
    for i in range(len(Matrix)):
        if(i%2!=0):
            continue
        else:
            try: #works for list of lists
                avgMatrix.append([sum(x)/2 for x in zip(Matrix[i],Matrix[i+1])])
            except:#work for list
                avgMatrix.append((Matrix[i]+Matrix[i+1])/2)

    return avgMatrix

def subMatrix(Matrix, rangoInf=None, rangoSup=None):
#in: list of lists, int, int
#out: list of lists
#fn: Make a sub matrix of Matrix between rangoInf and rangoSup.
    subMatrix=[]

    if(rangoInf==None):
        rangoInf=0
    if(rangoSup==None):
        rangoSup=len(Matrix[0])

    if(rangoInf<0 or rangoSup>(len(Matrix[0]))):
        sys.exit('Rangos fuera de los margenes '+str(rangoInf)+' '+str(rangoSup))
    
    for i in range(len(Matrix)):
        subMatrix.append(Matrix[i][rangoInf:rangoSup+1])

    return subMatrix

def subMatrixWL(Matrix,ejex, WLInf=None, WLSup=None):
#in: list of lists, list, int, int
#out: [list of lists, list]
#fn: Each row of Matrix is same lenght as ejex. Find WLInf and WLSup in ejex and then make a sub matrix with those limits.
    subMatrix=[]
    
    for i in range(len(ejex)):#round frequency values
        ejex[i]=int(ejex[i])

    if(WLInf==None):
        rangoInf=0
    else:
        rangoInf=ejex.index(WLInf)
    if(WLSup==None):
        rangoSup=len(Matrix[0])
    else:
        rangoSup=ejex.index(WLSup)
    
    if(rangoInf<0 or rangoSup>(len(Matrix[0]))):
        sys.exit('Rangos fuera de los margenes '+str(rangoInf)+' '+str(rangoSup))
    
    for i in range(len(Matrix)):
        subMatrix.append(Matrix[i][rangoInf:rangoSup])
    newEjeX=ejex[rangoInf:rangoSup]

    return [subMatrix,newEjeX]

def subMatrixWLMult(Matrix,ejex, WLInf1=None, WLSup1=None, WLInf2=None, WLSup2=None):
#in: list of lists, list, int, int, int, int
#out: [list of lists, list]
#fn: Each row of Matrix is same lenght as ejex. Find WLInf1, WLInf2, WLSup1 and WLSup2 in ejex and then make a sub matrix with those limits.
    subMatrix=[]
    
    for i in range(len(ejex)):
        ejex[i]=int(ejex[i])

    if(WLInf1==None):
        rangoInf1=0
    else:
        rangoInf1=ejex.index(WLInf1)
    if(WLSup1==None):
        rangoSup1=len(Matrix[0])
    else:
        rangoSup1=ejex.index(WLSup1)

    if(WLInf2==None):
        rangoInf2=0
    else:
        rangoInf2=ejex.index(WLInf2)
    if(WLSup2==None):
        rangoSup2=len(Matrix[0])
    else:
        rangoSup2=ejex.index(WLSup2)
    
    if(rangoInf1<0 or rangoSup1>(len(Matrix[0]))):
        sys.exit('Rangos fuera de los margenes '+str(rangoInf1)+' '+str(rangoSup1))
    if(rangoInf2<0 or rangoSup2>(len(Matrix[0]))):
        sys.exit('Rangos fuera de los margenes '+str(rangoInf2)+' '+str(rangoSup2))
    
    for i in range(len(Matrix)):
        subMatrix.append(Matrix[i][rangoInf1:rangoSup1]+Matrix[i][rangoInf2:rangoSup2])
    newEjeX=ejex[rangoInf1:rangoSup1]+ejex[rangoInf2:rangoSup2]
    
    return [subMatrix,newEjeX]

def ejex(path):
#in: string
#out: list
#fn: path of the objective folder. Make a vector(list) with the wavelenght of the measurements.    
    files=os.listdir(path)
    files.sort()
    
    text=open(path+'/'+files[1],'r')
    
    row=[]
    
    for line in text:
        if(is_letter(line[0])):
            continue
        line=line.replace('\t',' ') #tab for space
        line=line.replace(',','.')
        line=line[:len(line)-2] #to delete \n
        data=line.split(' ')
        row.append(float(data[0]))
    
    return row

def subEjeX(ejeX,WLInf=None, WLSup=None):
#in: list, int, int
#out: list
#fn: Make a sub vector between WaveLenghtInf and WaveLenghtSup  
    rangoInf=0
    rangoSup=len(ejeX)

    for i in range(len(ejeX)):
        ejeX[i]=int(ejeX[i])     
    if(WLInf!=None):
        rangoInf=ejeX.index(WLInf)
    if(WLSup!=None):
        rangoSup=ejeX.index(WLSup)

    return ejeX[rangoInf:rangoSup]

def pls(matrix,vector,factores,ejeX=None,printR2=False,grafico=False,name=' ',rango=' '):
#in:    matrix=list of lists, vector=list ,factores=int
#in(opt): ejeX=list, printR2=boolean, grafico=boolean, name=string,rango=string
#out: int
#fn: Execute Partial Least Squares with Matrix as Training Data, vector as Target Values and factores as number of iterations.
    Y=vector[:]
    X=matrix[:]
    
    X_model=[]
    X_val=[]
    Y_model=[]
    Y_val=[]
    
    Y_pred=[]


    if(len(X)!=len(Y)):
        sys.exit('Distinta cantidad de muestras entre Espectros y SS')

    for i in range(len(X)):
        if(i%2==0):
            X_model.append(X[i])
            Y_model.append(Y[i])
        else:
            X_val.append(X[i])
            Y_val.append(Y[i])
    
    pls=plsLib.PLS(factores)
    pls.learn(X_model,Y_model)

    for i in X_val:
        Y_pred.append(float(pls.pred(i)))

    slope, intercept, r_value, p_value, std_err = stats.linregress(Y_val,Y_pred)

    if(printR2==True):
        print 'R^2: '+str(r_value**2)
        print 'R^2: '+str(metrics.r2_corr(Y_val,Y_pred))+' MSE: '+str(metrics.mse(Y_val,Y_pred))

        auxPrint=[[Y_val[i],round(Y_pred[i],2)] for i in range(len(Y_val))]

        auxPrint.insert(0,['Validation','Prediction'])
        pp=pprint.PrettyPrinter(indent=1)
        pp.pprint(auxPrint)
        
    if(grafico==True):

        fig=plt.figure()
        ax1=plt.subplot2grid((2,2),(0,0),colspan=2)
        plt.plot(Y_val,'r')
        plt.plot(Y_pred,'g')
        plt.legend(('Validation','Prediction'),loc=0)
        plt.text(0.5,0.9,"R^2="+str(round(r_value**2,3)),horizontalalignment='center',verticalalignment='center',transform=ax1.transAxes,size='large',weight='heavy')
        plt.text(0.5,0.8,'MSE='+str(metrics.mse(Y_val,Y_pred)),horizontalalignment='center',verticalalignment='center',transform=ax1.transAxes,size='large',weight='heavy')
        if(name!=None):
            plt.title('Prediction of SS '+name+' '+rango, size='large',weight='normal', family='serif')

        xMax=max(max(Y_val),max(Y_pred))
        xMin=min(min(Y_val),min(Y_pred))

    
        ax2=plt.subplot2grid((2,2),(1,0))
        plt.scatter(Y_val,Y_pred)
        ax2.set_ylabel('Prediction', size='medium',weight='normal', family='serif')
        ax2.set_xlabel('Validation', size='medium',weight='normal', family='serif')
        plt.xlim(xMin,xMax)
        plt.ylim(xMin,xMax)
        plt.autoscale(enable=True,axis='both',tight='False')
        plt.axis('scaled')
        
        ax3=plt.subplot2grid((2,2),(1,1))
        
        plt.title('Fruits Spectra '+name+' '+rango, size='large',weight='normal', family='serif')
        
        if(ejeX!=None):
            plt.plot(ejeX,transpose(X))    
            ax3.set_xlabel('Longitud de onda [nm]', size='medium',weight='normal', family='serif')
        else:
            plt.plot(transpose(X))
        
        plt.show()

    return r_value**2 

def comPls(Matrix,Vector,factores):
#in: list of lists, list, int
#out: list
#fn: Compare PLS with number of iterations from 1 to factores.
    rcuadrado=[]
    for j in range(factores):
        rcuadrado.append(pls(Matrix,Vector,j+1))
    return rcuadrado
    # plot(rcuadrado)
    # show()    

def optRange(Matrix,Vector,Factores,ejeX=None):
#in: list of lists/string, list/string, int, list
#out: list
#fn: By setting Range limits, Span and Step find the wavlenght range who return the best R^2   
    try:
        #ipdb.set_trace()
        ejeX=ejex(Matrix)
        Matrix=getMatrix(Matrix)
        Vector=getVector(Vector)
    except:
        print 'No ha ingresado el path'        
    
    if(len(Matrix)==2*len(Vector)):
        Matrix=avgMatrix(Matrix)
        print 'Se ha detectado duplicidad en las muestras y se ha tomado el promedio en pares.'

    resultado=[]

    rows=len(Matrix)
    cols=len(Matrix[0])
    minR2=0.0

    RangeInf=1200
    RangeSup=3000
    
    spanInf=50
    spanSup=500
    spanStep=10

    stepInf=25
    stepSup=100
    stepStep=5

    for Span in range(spanInf,spanSup,spanStep):    

        for Step in range(stepInf,stepSup,stepStep):

            for j in range(RangeInf-1,RangeSup-(Span-1),Step):
                
                if (j==RangeInf-1 and Step==stepInf and Span==spanInf):#first iteration
                    ti=time.time()

                Xfil=subMatrix(Matrix,j,j+(Span-1))
                r2=pls(Xfil,Vector,Factores)
                
                if(r2>=minR2): #add the range only if R^2 >= min R^2 defined in the beggining (MinR2).
                    resultado.append((r2,ejeX[j],ejeX[j+(Span-1)],Span))
                
                if (j==RangeInf-1 and Step==stepInf and Span==spanInf):#first iteration. Compute the aprox time of execution.
                    tf=(time.time()-ti)*(RangeSup-RangeInf)/Step*(stepSup-stepInf)/stepStep*(spanSup-spanInf)/spanStep
                    print 'Tiempo estimado '+str(round(tf/60,3))+' min'
    
    resultado.sort(reverse=True)
    
    for i in range(len(resultado)):
        if(i>10):
            break
        print resultado[i]

    return resultado
