class Alu { 

    private:
		bitset<dataBus> PC, Const8, Const16, Const24, rc, ra, rb, r31, num, zero;
		bitset<addressBus> novoPC, novoRc;
		Registradores *regs;
		If *ifStage;
		Id *idStage;
		Controle *controle;
		Memoria *memoria;
		int rbInt; // Apenas usado para trabalhar com o bitset em função especifica ">> e <<", que exige um inteiro.
        bool ALUzero;
		bool overflow;
		bool carry;
        bool borrow;
		bool neg;
		bool continuar;
        void verificaNegativo(bitset<dataBus> rc);
		void verificaOverflow(bitset<dataBus> ra, string operacao);
		void converteBits(int operacao);
        bitset<dataBus> calculaBits(bitset<dataBus> ra, bitset<dataBus> rb, string operacao);

    public:
        Alu(Registradores *regs, If *ifStage, Id *idStage, Controle *controle, Memoria *memoria);
		~Alu();
        void instrucoesAritmeticas();
        void instrucoesDeDesvio();
        void instrucoesDeMemoria();
        void mostrarFlags();
		bool getOverflow()					{return overflow;};
		bitset<dataBus> getResultadoRa()	{return ra;};
		bitset<dataBus> getResultadoRc()	{return rc;};
		bitset<dataBus> getRetornoFuncao()	{return r31;};
		bool getHalt()						{return continuar;};
        
};

Alu::Alu(Registradores *regs, If *ifStage, Id *idStage, Controle *controle, Memoria *memoria) {

	this->regs = regs;
	this->ifStage = ifStage;
	this->idStage = idStage;
	this->controle = controle;
	this->memoria = memoria;
	
	rc = idStage->getRcValue();
	ra = idStage->getRaValue();
	rb = idStage->getRbValue();
	r31 = regs->getRegistrador(31);
	
	converteBits(1);

    ALUzero = 0;
    zero = 0;
    overflow = 0;
	carry = 0;
    borrow = 0;
	neg = 0;
	num = (65535);
	continuar = true;
}

Alu::~Alu() {

	cout << "alu morreu" << endl;
}

void Alu::instrucoesAritmeticas(){

	bool aux = 0;
	
	// SOMA INTEIRA
	if(controle->getAluctrl() == "add"){
		rc = calculaBits(ra, rb, "adicao");
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de add" << endl << endl;
	}
	
	// SUBTRAÇÃO INTEIRA
	if(controle->getAluctrl() == "sub"){
		rc = calculaBits(ra, rb, "subtracao");
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de sub" << endl << endl;
	}
	
	// ZERA
	if(controle->getAluctrl() == "zeros"){
		rc = 0;
		// nao causa overflow
		// neg por padrao é zero
		zero = rc;
		
		cout << "Eh uma instrucao de zeros" << endl << endl;
	}
	
	// XOR LÓGICO
	if(controle->getAluctrl() == "xor"){
		rc = ra ^ rb;
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;

		cout << "Eh uma instrucao de xor" << endl << endl;
	}
	
	// OR LÓGICO
	if(controle->getAluctrl() == "or"){
		rc = ra | rb;
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de or" << endl << endl;
	}
	
	// NOT
	if(controle->getAluctrl() == "passnota"){
		rc = ~ra;
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de passnota" << endl << endl;
	}
	
	// AND LÓGICO
	if(controle->getAluctrl() == "and"){
		rc = ra & rb;
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de and" << endl << endl;
	}
	
	// SHIFT ARITMÉTICO PARA A ESQUERDA
	if(controle->getAluctrl() == "asl"){
		aux = ra[31]; 
		rc = ra << rbInt;
		rc[31] = aux;
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de asl" << endl << endl;
	}
	
	// SHIFT ARITMÉTICO PARA A DIREITA
	if(controle->getAluctrl() == "asr"){
		aux = ra[31]; 
		rc = ra >> rbInt;
		rc[31] = aux;
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de asr" << endl << endl;
	}

	// SHIFT LÓGICO PARA A ESQUERDA
	if(controle->getAluctrl() == "lsl"){
		rc = ra << rbInt;
		rc[31] = aux;
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de lsl" << endl << endl;
	}
	
	// SHIFT LÓGICO PARA A DIREITA
	if(controle->getAluctrl() == "lsr"){
		rc = ra >> rbInt;
		rc[31] = aux;
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de lsr" << endl << endl;
	}
	
	// COPIA
	if(controle->getAluctrl() == "passa"){
		rc = ra;
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de passa" << endl << endl;
	}
	
	// CARREGA CONSTANTE NOS 2 BYTES MAIS SIGNIFICATIVOS
	if(controle->getAluctrl() == "lch"){
		rc = (Const16 << 16) | (rc & num);
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de lch" << endl << endl;
	}
	
	// CARREGA CONSTANTE NOS 2 BYTES MENOS SIGNIFICATIVOS
	if(controle->getAluctrl() == "lcl"){
		rc = Const16 | (rc & num);
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de lcl" << endl << endl;
	}

	// Slt
	if(controle->getAluctrl() == "slt"){
		
	    bool result = false; 

    	for (int i = 31; i >= 0 && !result ; i--) {
    		if (ra[i] < rb[i]) {
        		rc = result;
        	} 
			else if (ra[i] > rb[i]) {
            	rc = result;
        	}
    	}

		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de slt" << endl << endl;
	}

	// Slti
	if (controle->getAluctrl() == "slti") {

		bool result = false;

    	for (int i = 31; i >= 0 && !result ; i--) {
    		if (ra[i] < Const8[i]) {
        		rc = result;
        	} 
			else if (ra[i] > Const8[i]) {
            	rc = result;
        	}
    	}

    	// não causa overflow
    	verificaNegativo(rc);
    	zero = rc;

    	cout << "Eh uma instrucao de slti" << endl << endl;
	}

	// Smt
	if (controle->getAluctrl() == "smt") {

		bool result = false; 

    	for (int i = 31; i >= 0 && !result ; i--) {
    		if (ra[i] > rb[i]) {
        		rc = result;
        	} 
			else if (ra[i] < rb[i]) {
            	rc = result;
        	}
    	}
 		
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;

    	cout << "Eh uma instrucao de smt" << endl << endl;
	}

	// Inc
	if (controle->getAluctrl() == "inc") {

 		rc = calculaBits(rc, 1, "adicao");
    	verificaNegativo(rc);
    	zero = rc;

    	cout << "Eh uma instrucao de inc" << endl << endl;
	}

	// Dec
	if (controle->getAluctrl() == "dec") {

 		rc = calculaBits(rc, 1, "subtracao");
    	verificaNegativo(rc);
    	zero = rc;

    	cout << "Eh uma instrucao de dec" << endl << endl;
	}

	// Addi
	if(controle->getAluctrl() == "addi"){
		rc = calculaBits(ra, Const8, "adicao");
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de addi" << endl << endl;
	}

	// Subi
	if(controle->getAluctrl() == "subi"){
		rc = calculaBits(ra, Const8, "subtracao");
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de subi" << endl << endl;
	}

	// Nand
	if (controle->getAluctrl() == "nand") {
    	rc = ~(ra & rb);
    	// não causa overflow
    	verificaNegativo(rc);
    	zero = rc;

    	cout << "Eh uma instrucao de nand" << endl << endl;
	}

	// Nor
	if(controle->getAluctrl() == "nor"){
		rc = ~(ra | rb);
		// nao causa overflow
		verificaNegativo(rc);
		zero = rc;
		
		cout << "Eh uma instrucao de nor" << endl << endl;
	}

	// halt
	if(controle->getAluctrl() == "halt - saída de sistema") {
		continuar = false;

		cout << "Eh uma instrucao de halt" << endl << endl;
	}
	
	// Verifica se algum resultado corresponde a 0 para acionar a flag
	if(zero == 0){
		ALUzero = 1;
	}
}

void Alu::instrucoesDeDesvio(){

	// JUMP AND LINK
	if(controle->getAluctrl() == "jal"){
		r31 = PC; 
		PC = Const24;	
		cout << "Eh uma instrucao de jal" << endl << endl;
	}
	
	// JUMP REGISTER
	if(controle->getAluctrl() == "jr"){
		PC = r31;	
		cout << "Eh uma instrucao de jr" << endl << endl;
	}
	
	//  JUMP SE IGUAL (BEQ)
	if(controle->getAluctrl() == "beq"){
		if(ra == rb)
			PC = Const8;

		cout << "Eh uma instrucao de beq" << endl << endl;
	}
	
	//  JUMP SE DIFERENTE (BNE)
	if(controle->getAluctrl() == "bne"){
		if(ra != rb){
			PC = Const8;
		}

		cout << "Eh uma instrucao de bne" << endl << endl;
	}
	
	//  JUMP INCONDICIONAL e ADRESS
	if(controle->getAluctrl() == "j" || controle->getAluctrl() == "adress"){
		PC = Const24;
		cout << "Eh uma instrucao de j ou adress" << endl << endl;
	}
	
	// Alterar o PC e o r31 já que eles foram modificados.

	converteBits(2);
	ifStage->desviaPc(novoPC);
	regs->setRegistrador(r31, 31); 
	
}

void Alu::instrucoesDeMemoria(){
	
	// LOAD WORD (vou refazer)
	if(controle->getAluctrl() == "load"){
		//rc = tenho que setar na memoria o ra;
		cout << "Eh uma instrucao de load" << endl << endl;
	}
	
	// STORE WORD (vou refazer)
	if(controle->getAluctrl() == "store"){

		if (controle->getRegwrite() == 0  && controle->getMemwrite() == 1){

			converteBits(3);

			memoria->armazenarDado(ra, novoRc);
			
		}
	}
		//tenho que setar na memoria o rc = ra;
		cout << "Eh uma instrucao de store" << endl << endl;
	}
}

void Alu::converteBits(int operacao) {

	if(operacao == 1){ // Converte para 32

    	bitset<16> PC = ifStage->getPc();

		bitset<8> Const8 = idStage->getConst8();
    	bitset<16> Const16 = idStage->getConst16();
    	bitset<24> Const24 = idStage->getConst24();

    	for (int i = 0; i < 16; i++) {
        	this->PC[i] = PC[i];
    	}

		for (int i = 0; i < 8; i++) {
        	this->Const8[i] = Const8[i];
    	}

    	for (int i = 0; i < 16; i++) {
        	this->Const16[i] = Const16[i];
    	}

		for (int i = 0; i < 24; i++) {
        	this->Const24[i] = Const24[i];
    	}

		rbInt = 0;

		for (int i = 0; i < 32; i++) { // Transforma rb bitset em int
        	rbInt <<= 1; 
        	rbInt |= rb[i];
    	}		

		if (rc[31]) { // Caso o valor do bitset seja negativo, o inteiro dele também deve ser negativo. Faz o mesmo que verificaNegativo(), mas não mexe na variavel neg.
    		rbInt = -rbInt;
		}

	}

	else if(operacao == 2){ // Converte para 16
		
		for (int i = 0; i < 16; i++) {
        	novoPC[i] = this->PC[i];
    	}

	}

	else if(operacao == 3){

		for (int i = 0; i < 16; i++) {
        	novoRc[i] = rc[i];
    	}

	}

}

bitset<dataBus> Alu::calculaBits(bitset<dataBus> ra, bitset<dataBus> rb, string operacao){
	
	// adicao
	if((operacao == "adicao")){
		
        bitset<dataBus> sum;
	
		for (int i = 0; i < dataBus; i++) {
			sum[i] = ra[i] ^ rb[i] ^ carry;  // Soma bit a bit com carry
			carry = (ra[i] & rb[i]) | (carry & (ra[i] ^ rb[i]));  // Calcula o carry para o próximo bit
		}

		verificaOverflow(ra, operacao);
        return sum;
    }
		
	// subtracao
	if((operacao == "subtracao")){
		bitset<dataBus> difference;
		
		for (int i = 0; i < dataBus; i++) {
			difference[i] = ra[i] ^ rb[i] ^ borrow;  // Subtrai bit a bit com borrow
			borrow = (!ra[i] & rb[i]) | (borrow & (ra[i] ^ rb[i]));  // Calcula o borrow para o próximo bit
		}

		verificaOverflow(ra, operacao);
        return difference;
    }

    return 0;
			
}

void Alu::verificaOverflow(bitset<dataBus> ra, string operacao){

	if(operacao == "adicao"){
		overflow = carry;
	}
	
	if(operacao == "subtracao"){
		overflow = (borrow != ra[31]);
	}

}

void Alu::verificaNegativo(bitset<dataBus> rc){

	// Verifica se o bit mais significativo é 1 (Se é negativo)
	if (rc[31]) {
        neg = 1;
    }

}

void Alu::mostrarFlags(){
	
	cout << "Flags da ALU e endereco de desvio: " << endl;
	cout << "\tFlag de Zero: "		<< ALUzero << endl;
    cout << "\tCarry: " 			<< carry << endl;
    cout << "\tNegativo: " 			<< neg << endl;
	cout << "\tSinal de Overflow: " << overflow << endl;
	cout << "\tCalculo do endereco de desvio (valor de resultado): " << rc << endl;
}
