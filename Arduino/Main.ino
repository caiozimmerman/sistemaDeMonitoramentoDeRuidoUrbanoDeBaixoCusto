/*
 * Main.ino
 *  Autor: Caio Gomes da Silva Zimmerman
 *  Projeto: Desenvolvimento de um sistema de monitoramento de ruído urbano de baixo custo
 *  Descrição: Programa para o Arduíno MKR1000 que configura coleta as amostras do sinal
 *  gerado pelo eletreto e as envia pela internet para o servidor.
 */
#include <math.h>
#include <WiFi101.h>

// Configurações da rede WiFi
char rede[] = "";
char senha[] = "";
// Configurações do Servidor
char servidor[] = "";
int porta = 6000;
int status = WL_IDLE_STATUS;
// Constantes de uso no programa
const int tamanhoDoVetor = 153; // Tamanho do vetor usado para armazenar o quadrado das amostras
const double a = 26.96; // Coeficiente usado no cálculo do nível de ruído
const double b = 10.84; // Coeficiente usado no cálculo do nível de ruído
// Constantes de configuração dos registradores
const byte clockGenerico = 3; // Definição de qual clock genérico será utilizado pelo conversor AD
const byte prioridadeDaInterrupcao = 0; // Definição da prioridade das interrupções feita pelo conversor AD - 0 é a maior prioridade
const int fatorDeDivisao = 1; // Fator de Divisao usado pelo clock genérico
const int moduloDoConversor = 30; // Numero do modulo do conversor AD
// Variáveis de uso no programa
volatile int aux=0; // Variável usada na coleta dos valores lido pelo conversor AD
volatile unsigned int somaDeQuadrados[tamanhoDoVetor]; // Vetor usado para armazenar o quadrado das amostras
volatile int amostras = 0; // Armazena o número de amostras coletadas em 1 s
volatile int conversoesSomadas = 0; // Conta quantos valores foi armazenado em 1 int (valor máximo é 256)
volatile int indice = 0; // Indice do vetor de quadrados das amostras
double valorRMS = 0.0;
bool pare = false; // Usada para indicar quando as coletas devem ser coletadas
double nivelDeRuido = 0.0;
int tempo=0;

void setup() {
  // Inicializa o Serial e espera a porta abrir
  Serial.begin(9600);
  while(!Serial){}
  // Verificar a presença do módulo WiFi
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("Modulo WiFi não esta presente.");
    while (true);
  }
  // Tenta conectar à rede WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Tentando conectar a rede: ");
    Serial.println(rede);
    status = WiFi.begin(rede, senha);
    // Espera 10s pela conexão
    delay(10000);
  }
  // Exibe o status do WiFi no Monitor Serial
  Serial.print("Rede: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("Endereço IP: ");
  Serial.println(ip);
  long forcaDoSinal = WiFi.RSSI();
  Serial.print("Força do Sinal (RSSI):");
  Serial.print(forcaDoSinal);
  Serial.println(" dBm");
  // Configura as portas, o clock, o conversor e as interrupcoes para a conversão AD
  configurarPinos();
  configurarClock();
  configurarConversorAD();
  configurarInterrupcoes();
  // Zera o vetor de quadrados
  for(indice=0;indice<tamanhoDoVetor;indice++){
    somaDeQuadrados[indice]=0;
  }
  indice=0;
  // Inicia o conversor AD e marca o tempo
  tempo = micros();
  iniciarConversorAD();
}

void loop() {
  // Verifica se 1 s passou desde a ultima conversão
  if((micros() - tempo) >= 1000000){
    // Para de coletar amostras
    pare = true;
    // Calcula o valor RMS das amostras
    for(indice=0;indice<tamanhoDoVetor;indice++){
      valorRMS += (somaDeQuadrados[indice]/amostras);
    }
    valorRMS=sqrt(valorRMS);
    // Calcula o nível de ruído naquele segundo
    nivelDeRuido = a*log10(valorRMS) + b;
    // Zera todas as variáveis usadas na coleta e inicia um novo ciclo
    for(indice=0;indice<tamanhoDoVetor;indice++){
      somaDeQuadrados[indice]=0;
    }
    amostras = 0;
    conversoesSomadas = 0;
    indice = 0;
    valorRMS = 0.0;
    tempo = micros();
    pare = false;
    // Envia o nivel de ruido para o servidor
    enviarDados(nivelDeRuido);
  }
}

/*
 * Função responsável por enviar o pacote de dados ao servidor
 */
void enviarDados(float nivel){
  // Tenta conectar ao servidor
  WiFiClient cliente;
  if (cliente.connect(servidor, porta)) {
    // Caso conecte, envia os dados medidos ao servidor através do método POST
    String conteudo = "NIVEL_DE_RUIDO=" + String(nivel,1) + "&ID=1";
    cliente.println("POST /TCC/adicionar.php HTTP/1.1");
    cliente.println("Host: 192.168.0.105");
    cliente.println("Connection: close");
    cliente.println("Content-Type: application/x-www-form-urlencoded");
    cliente.println("Content-Length: " + String(conteudo.length()));
    cliente.println();
    cliente.println(conteudo);
    delay(100);
    cliente.stop();
    Serial.println("Dados enviados com sucesso: " + conteudo);
  } else {
    // Caso não conecte, exibe mensagem de erro
    Serial.println("Erro ao enviar dados...");
  }
}

/*
 * Função que configura o pino utilizada pelo conversor AD
 */
void configurarPinos() {
  // Seleciona o pino A3 para realizar a conversão
  REG_PORT_DIRCLR1 = PORT_PA04;
}

/*
 * Função que configura o clock genérico a ser usado pelo módulo do conversor AD
 */
void configurarClock() {
  // Habilita o clock APBC para o ADC
  REG_PM_APBCMASK |= PM_APBCMASK_ADC;
  // Configura um fator de divisão para o clock genérico utilizado
  GCLK->GENDIV.reg |= GCLK_GENDIV_ID(clockGenerico)| GCLK_GENDIV_DIV(fatorDeDivisao);
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);  
  // Configura o clock de 48 MHz como gerador do clock genérico
  GCLK->GENCTRL.reg |= GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(clockGenerico);
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
  // Habilita o clock, define o seu número e o número do módulo para o que ele vai
  GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(clockGenerico) | GCLK_CLKCTRL_ID(moduloDoConversor);
  while (GCLK->STATUS.bit.SYNCBUSY);
}

/*
 * Função que configura o conversor AD, incluindo a resolução e a taxa de amostragem
 */
void configurarConversorAD(){
  // Seleciona o VCC do Arduíno como referência
  ADC->REFCTRL.reg |= ADC_REFCTRL_REFSEL_INTVCC1;
  // Defina que não usará a média dos valores, a leitura será de 1 amostra
  ADC->AVGCTRL.reg |= ADC_AVGCTRL_ADJRES(0) | ADC_AVGCTRL_SAMPLENUM_1;
  // Pode ser utilizada para economizar ciclos de clock, mas não é usado nesse programa
  REG_ADC_SAMPCTRL |= ADC_SAMPCTRL_SAMPLEN(0);
  // Controle do ganho do ADC (Valor padrão) e referências de tensão para o conversor
  ADC->INPUTCTRL.reg |= ADC_INPUTCTRL_GAIN_DIV2 | ADC_INPUTCTRL_MUXNEG_GND | ADC_INPUTCTRL_MUXPOS_PIN4;
  while (REG_ADC_STATUS & ADC_STATUS_SYNCBUSY);
  // Define a divisão do clock, a resolução e o modo Free Run
  ADC->CTRLB.reg |= ADC_CTRLB_PRESCALER_DIV32 | ADC_CTRLB_RESSEL_12BIT | ADC_CTRLB_FREERUN;
  while (REG_ADC_STATUS & ADC_STATUS_SYNCBUSY);
  // Desabilita o modo Window monitor
  ADC->WINCTRL.reg = ADC_WINCTRL_WINMODE_DISABLE;
  while(ADC->STATUS.bit.SYNCBUSY);
  // Inicia o conversor que o evento ocorrer
  ADC->EVCTRL.reg |= ADC_EVCTRL_STARTEI;
  while (ADC->STATUS.bit.SYNCBUSY);
  // Habilita o ADC
  ADC->CTRLA.reg |= ADC_CTRLA_ENABLE;
  while (ADC->STATUS.bit.SYNCBUSY);

}

/*
 * Função que configura a interrupção que é desencadeada
 */
void configurarInterrupcoes() {
  // Habilita a interrupção de conversão feita
  ADC->INTENSET.reg |= ADC_INTENSET_RESRDY;
  while (ADC->STATUS.bit.SYNCBUSY);
  // Habilita interrupções pelo conversor AD
  NVIC_EnableIRQ(ADC_IRQn);
  // Define a prioridade da interrupação
  NVIC_SetPriority(ADC_IRQn, prioridadeDaInterrupcao);
}

/*
 * Função que determina a ISR (Interruption Service Routine) que é chamada cada vez que o conversor faz uma leitura
 */
void ADC_Handler() {
  // Verifica se é para coletar amostras
  if(!pare) {
    // Caso afirmativo lê o valor gerado e o eleva ao quadrado
    aux = REG_ADC_RESULT;
    aux = aux - 2048;
    aux = aux*aux;
    // Verifica em qual elemento do vetor a leitura deve ser gravada
    if(conversoesSomadas==256){
      indice++;
      conversoesSomadas=0;
    }
    // Armazena no vetor no indice correto
    somaDeQuadrados[indice] = somaDeQuadrados[indice]+aux;
    amostras++;
    conversoesSomadas++;
  }
  // É necessário resetar a interrupção
  ADC->INTFLAG.reg = ADC_INTENSET_RESRDY;
}

/*
 * Gatilho de software que inicia o conversor AD no modo Free Run
 */
void iniciarConversorAD() {
  ADC->SWTRIG.reg |= ADC_SWTRIG_START;
}
