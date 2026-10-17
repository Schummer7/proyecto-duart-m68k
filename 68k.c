

    ORG $0
    DC.L $8000 *Pila
    DC.L INICIO *PC counter

    ORG $400


MR1A:    EQU    $EFFC01
MR2A:    EQU    $EFFC01
SRA:     EQU    $EFFC03
CSRA:    EQU    $EFFC03
CRA:     EQU    $EFFC05
RBA:     EQU    $EFFC07
TBA:     EQU    $EFFC07
ACR:     EQU    $EFFC09         * pag 41 pdf Estan los controles A y B
IMR:     EQU    $EFFC0B
ISR:     EQU    $EFFC0B
IVR:     EQU    $EFFC19
MR1B:    EQU    $EFFC11
MR2B:    EQU    $EFFC11
CSRB:    EQU    $EFFC13
SRB:    EQU    $EFFC13
CRB:     EQU    $EFFC15
TBB:     EQU    $EFFC17
RBB:     EQU    $EFFC17

ALIN DS.B 1    * inicio dir par
IMRSECUN DS.B 1   * copia del IMR


     
INICIO:
    MOVE.W  #$2700,SR     
    BSR     INIT            *  DUART 
    MOVE.W  #$2000,SR  

MAIN:
    BRA MAIN
    

INIT:
                         *Configuramos velocidad,bits e incializamos los 4 buffer entre otras cosas
    MOVE.L  A1,-(SP)
    MOVEA.L #$100,A1
    MOVE.L  #RTI,(A1)   * rutina RTI
    MOVE.B #$00,ACR    * Utilizamos el conjunto 1 para la velocidad
    MOVE.B #$CC,CSRA    * seleccionamos la velocidad en las 4 lineas
    MOVE.B  #$CC,CSRB
    MOVE.B  #$10,CRA
    MOVE.B  #$03,MR1A   
    MOVE.B  #$00,MR2A 
    MOVE.B #$05,CRA
    MOVE.B  #$10,CRB
    MOVE.B  #$03,MR1B     * Configuras el modo de los 8 bits
    MOVE.B  #$00,MR2B      
    MOVE.B  #$05,CRB
    MOVE.B  #$40,IVR
    MOVE.B  #$22,IMRSECUN
    MOVE.B  IMRSECUN,IMR   
    BSR INI_BUFS
    MOVE.L  (SP)+,A1  
    RTS
  


SCAN:                                      *VAcia el buffer interno y devuelve los caracteres leidos
    LINK    A6,#0
    MOVEM.L D1-D7/A0-A5,-(SP)
    CLR.L D1
    CLR.L D2 
    CLR.L D3      * al utilizar solo la parte baja por si en la alta hay algun resto
    MOVE.L 8(A6),A0 * Buffer
    MOVE.W 12(A6),D1 *Descriptor
    MOVE.W 14(A6),D2   * Tamaño
    MOVE.w D2,D3
    CMP.W #0,D1
    BEQ SIGUESCAN
    CMP.W #1,D1
    BEQ SIGUESCANB
    MOVE.L  #$FFFFFFFF,D0
    BRA TERMINASCAN

SIGUESCAN:
	CMP.w #0,D2
	BEQ FINSCAN
    CLR.L D0
    BSR LEECAR
    CMP.L #$FFFFFFFF,D0
    BEQ FINSCAN
    MOVE.B D0,(A0)+
    SUBQ.W #1,D2
    BEQ FINSCAN
    BRA SIGUESCAN



SIGUESCANB:
	CMP.w #0,D2
	BEQ FINSCAN
    MOVE.L #1,D0
    BSR LEECAR
    CMP.L #$FFFFFFFF,D0 
    BEQ FINSCAN
    MOVE.B D0,(A0)+
    SUBQ.W #1,D2
    BEQ FINSCAN
    BRA SIGUESCANB

FINSCAN:
    SUB.W D2,D3
    MOVE.L D3,D0

TERMINASCAN: 
    MOVEM.L (SP)+,D1-D7/A0-A5
    UNLK A6
    RTS


PRINT:                                  *Se inserta en el buffer de transmision
	LINK    A6,#0
	MOVEM.L D1-D7/A0-A5,-(SP)
	CLR.L D1
	CLR.L D2
	CLR.L D3
	CLR.L D4
	MOVE.L 8(A6),A0 * Buffer
	MOVE.W 12(A6),D1 *Descriptor
	MOVE.W 14(A6),D2 * Tamaño
	MOVE.w D2,D3
	CMP.W #0,D1
	BEQ SIGUEPRINT
	CMP.W #1,D1
	BEQ SIGUEPRINT
	MOVE.L  #$FFFFFFFF,D0
	BRA TRASMIPRINT
	
SIGUEPRINT:
	CMP.W #0,D2
	BEQ FINPRINT
	MOVE.W D1,D4
	ADD.W #2,D4 

BUCLEPRINT:
	MOVE.L D4,D0
	CLR.L D1
	MOVE.B (A0)+,D1
	BSR ESCCAR
	CMP.L #$FFFFFFFF,D0 
	BEQ FINPRINT
	SUBQ.W #1,D2
	BEQ FINPRINT
	BRA BUCLEPRINT

FINPRINT:
	SUB.W D2,D3
	MOVE.L D3,D0
	CMP.W #0,D3
	BEQ TRASMIPRINT
	
	MOVE.W 12(A6),D1 * Descriptor
	CMP.W #0,D1
	BEQ ZONAPRINTA

ZONAPRINTB:
	MOVE.w SR,D6
	MOVE.w #$2700,SR * Inhibe
	BSET #4,IMRSECUN   * esto es porque la imr es solo de escritura
	MOVE.B IMRSECUN,IMR
	MOVE.w D6,SR
	BRA TRASMIPRINT
	
ZONAPRINTA:
	MOVE.w SR,D6
	MOVE.w #$2700,SR * Inhibe
	BSET #0,IMRSECUN   * esto es porque la imr es solo de escritura
	MOVE.B IMRSECUN,IMR
	MOVE.w D6,SR
	BRA TRASMIPRINT


TRASMIPRINT:
	MOVEM.L (SP)+,D1-D7/A0-A5
	UNLK A6
	RTS

RTI:
	MOVEM.L D0-D7/A0-A6,-(SP) * La logica de esto es comprobar  que interrupcion es y solucionar esa misma, procesa en un bucle todas las fuentes activas hasta encontrarlo

EMPEZAR:
	MOVE.B ISR,D7
	AND.B IMRSECUN,D7
ENTRADAA:
	BTST #1,D7
	BEQ ENTRADAB
	CLR.L D1
	CLR.L D0
	MOVE.B RBA,D1
	BSR ESCCAR *LLamamos a esta subrutina pasando los valores en D0 y D1
	CMP.L #$FFFFFFFF,D0 
	BEQ FINRTI
	BRA EMPEZAR

ENTRADAB:
	BTST #5,D7
	BEQ SALIDAB
	CLR.L D1
	MOVE.L #1,D0
	MOVE.B RBB,D1
	BSR ESCCAR   
	CMP.L #$FFFFFFFF,D0 
	BEQ FINRTI		
	BRA EMPEZAR	       *LLamamos a esta subrutina pasando los valores en D0 y D1, igual pero cambiando el buffer al B 


SALIDAB:
	BTST #4,D7
	BEQ SALIDAA
	MOVE.L #3,D0
	BSR LEECAR
	CMP.L #$FFFFFFFF,D0 
	BEQ BORRARB
	MOVE.B D0,TBB
	BRA EMPEZAR

BORRARB:
	BCLR #4,IMRSECUN
	MOVE.B IMRSECUN,IMR
	BRA EMPEZAR



SALIDAA:
	BTST #0,D7
	BEQ FINRTI
	MOVE.L #2,D0
	BSR LEECAR
	CMP.L #$FFFFFFFF,D0 
	BEQ BORRAA
	MOVE.B D0,TBA
	BRA EMPEZAR

BORRAA:
	BCLR #0,IMRSECUN
	MOVE.B IMRSECUN,IMR
	BRA EMPEZAR


FINRTI:
	MOVEM.L (SP)+,D0-D7/A0-A6
	RTE


	INCLUDE bib_aux.s
