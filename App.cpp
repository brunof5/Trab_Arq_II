#include <exception>
#include "codes/Processador.cpp"

int main() {

    Processador *uflaRisc = NULL;

    try{
        uflaRisc = new Processador();

        uflaRisc->executar();

        delete uflaRisc;
    }
    catch(int erro) {
        cout << "ERRO " << erro << "!" << endl;
        switch (erro) {

            case 1:
                cout << "O arquivo não foi encontrado!" << endl;
                break;

            case 2:
                cout << "O valor de address é inválido!" << endl;
                break;

            case 3:
                cout << "O valor do PC foi ultrapassado!" << endl;
                break;

            case 4:
                cout << "O arquivo possui uma instrução inválida!" << endl;
                break;

            case 5:
                cout << "Acesso indevido na memória!" << endl;
                break;

            case 6:
                cout << "O registrador lido não pertence ao conjunto de registradores existentes!" << endl;
                break;

            case 7:
                cout << "Tentativa de setar algum valor no registrador zero!" << endl;
                break;

            case 8:
                cout << "Tentativa de setar algum valor no registrador reservado ao OS!" << endl;
                break;

            case 9:
                cout << "Tentativa de escrever em um registrador inexistente!" << endl;
                break;

            case 10:
                cout << "O valor setado no PC é inválido!" << endl;
                break;

            case 11:
                cout << "O valor setado no registrador é inválido (está fora do range de 32 bits)!" << endl;
                break;

            case 12:
                cout << "Overflow da adição ou subtração!" << endl;
                break;

            case 13:
                cout << "Algum operando de alguma instrução do código está fora do range de 8 bits!" << endl;
                break;

            case 14:
                cout << "Algum operando de alguma instrução do código está fora do range de 16 bits!" << endl;
                break;

            case 15:
                cout << "Algum operando de alguma instrução do código está fora do range de 24 bits!" << endl;
                break;

            default:
                cout << "Erro desconhecido encontrado!" << endl;
                break;
        }
    }

    return 0;
}
