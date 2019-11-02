/*  scriptPrincipal.js
 *  Autor: Caio Gomes da Silva Zimmerman
 *  Projeto: Desenvolvimento de um sistema de monitoramento de ruído urbano de baixo custo
 *  Descrição: Funções da página principal da Interface ao usuário
 */

// Váriaveis globais
var mapa;
var marcador;
var janelaDeInformacaoDoSensor;
var graficoEstaDesenhado = false;
var vetorDeNivelDeRuido;

/*
    Função que configura e exibe o mapa na página
*/
function initMap() {
    // Latitude e Longitude do LSI
    var LatELonDoSensor = {lat: -19.8698959, lng: -43.9617647};
    // Cria um objeto do tipo mapa centrado no LSI
    mapa = new google.maps.Map(document.getElementById('mapa'), {
        center: LatELonDoSensor,
        zoom: 20
    });
    // Cria um marcador no LSI
    marcador = new google.maps.Marker({
        position: LatELonDoSensor,
        icon: {url: "http://maps.google.com/mapfiles/ms/icons/green-dot.png"},
        map: mapa,
        title: 'Clique para ver o Nivel de Ruido'
    });
    // Adiciona uma janela de Informações ao marcador
    janelaDeInformacaoDoSensor = new google.maps.InfoWindow();
        janelaDeInformacaoDoSensor.setContent('SPL');
        janelaDeInformacaoDoSensor.setPosition(LatELonDoSensor);        
    marcador.addListener('click', function() {
        janelaDeInformacaoDoSensor.open(mapa,marcador);
    });
    janelaDeInformacaoDoSensor.addListener('closeclick', function() {
        fecharGrafico();
    });
}

/*
    Função a ser executada no momento em que a página termina de ser carregada e configura todas as funcionalidades da página
*/
$(document).ready(function(){
    // Abre o mapa
    initMap();
    // Configura a página para atualizar o nível de ruído do sensor a cada 1s
    setInterval(recuperarUltimaMedida, 1000);
    // Carrega a API do gráfico e prepara o vetor com os últimos 60 valores para exibição
    google.charts.load("current", {packages:["corechart"]});
    vetorDeNivelDeRuido = new Array();
    for (i = 0; i < 60; i++) { 
        vetorDeNivelDeRuido.push(40);
    }
})
/*
    Função que utiliza AJAX para recuperar o último valor do sensor
*/
function recuperarUltimaMedida() {
    // Cria uma váriavel para o AJAX e configura os parâmetros
    var http = new XMLHttpRequest();
    var parametros = 'ID=1';
    http.open('POST', 'selecionar.php', true);
    http.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    // Função a ser chamada no momento em que o servidor responde à requisição
    http.onreadystatechange = function() {
        if(http.readyState == 4 && http.status == 200) {
            // Lê o valor e muda a cor do marcador de acordo com o valor
            var nivelDeRuido = parseFloat(this.responseText);
            if(nivelDeRuido<70){
                marcador.setIcon({url: "http://maps.google.com/mapfiles/ms/icons/green-dot.png"});
            }
            if(nivelDeRuido>=70 && nivelDeRuido<85){
                marcador.setIcon({url: "http://maps.google.com/mapfiles/ms/icons/yellow-dot.png"});
            }
            if(nivelDeRuido>=85){
                marcador.setIcon({url: "http://maps.google.com/mapfiles/ms/icons/red-dot.png"});
            }
            // Modifica o conteudo da janela de informações do marcador
            var conteudo = '<div id="conteudo">' +
                                    '<p id="nivelDeRuido">Nível de Ruído: ' + this.responseText + 'dB(A)</p>'+
                                    '<p id="linkParaGrafico" onclick="abrirGrafico()">Gráfico</p>' +
                                    '<p id="linkParaRegistros" onclick="openRecordPage()">Registros</p>' +
                                '</div>';
            janelaDeInformacaoDoSensor.setContent(conteudo);
            // Atualiza o vetor com os últimos 60 valores medidos
            for (i = 0; i < 59; i++) { 
                vetorDeNivelDeRuido[i] = vetorDeNivelDeRuido[i+1];
            }
            vetorDeNivelDeRuido[59] = nivelDeRuido;
            if(graficoEstaDesenhado){
                desenharGrafico();
            }
        }
    }
    // Envia a requisição
    http.send(parametros);
}

/*
    Abre o Gráfico que mostra a evolução dos valores nos últimos 60s
*/
function abrirGrafico() {
    $("#recipienteDeGrafico").css("display", "block");
    graficoEstaDesenhado = true;
    desenharGrafico();
}

/*
    Fecha o Gráfico que mostra a evolução dos valores nos últimos 60s
*/
function fecharGrafico() {
    $("#recipienteDeGrafico").css("display", "none");
    graficoEstaDesenhado = false;
}

/*
    Desenha o Gráfico que mostra a evolução dos valores nos últimos 60s
*/
function desenharGrafico() {
    // Configura os eixos do gráfico
    var dadosDoGrafico = new google.visualization.DataTable();
    dadosDoGrafico.addColumn('number', 'Medida');
    dadosDoGrafico.addColumn('number', 'Nivel de Ruido');
    var i;
    // Popula o vetor com os valores do gráfico
    for (i = 0; i < 60; i++) { 
        dadosDoGrafico.addRow([59-i,vetorDeNivelDeRuido[i]]); 
    }
    // Determina as características do gráfico e o desenha
    var opcoesDoGrafico = {
        title: 'Nivel de Ruido',
        legend: {position: 'none'},
        vAxis: {viewWindow: {min: 40, max: 115}},
    };
    var grafico = new google.visualization.LineChart(document.getElementById('grafico'));
    grafico.draw(dadosDoGrafico,opcoesDoGrafico);
}

/*
    Abre a página de histórico dos sensores
*/
function openRecordPage() {
    var formulario = document.getElementById('formularioDeRegistros');
    formulario.ID.value = '1';
    formulario.submit();
}