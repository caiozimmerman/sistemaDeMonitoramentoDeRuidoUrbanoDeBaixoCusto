/*  scriptRegistros.js
 *  Autor: Caio Gomes da Silva Zimmerman
 *  Projeto: Desenvolvimento de um sistema de monitoramento de ruído urbano de baixo custo
 *  Descrição: Funções da página de dados históricos da Interface ao usuário
 */

/*
    Carrega a API do calendario para selecionar datas
*/
$( function() {
    $( "#calendario" ).datepicker({
  		dateFormat: "yy-mm-dd"
		});
  } );

/*
    Carrega a API do gráfico
*/
$(document).ready(function(){
    google.charts.load("current", {packages:["corechart"]});
})

/*
    Recupera os dados históricos do sensor através do AJAX e os envia para a função de desenhar gráficos
*/
function desenharGraficoDeRegistros() {
    // Cria uma váriavel para o AJAX e configura os parâmetros
    var http = new XMLHttpRequest();
    var id = $("#idSensor").text();
	var data = $('#calendario').datepicker().val();
    var parametros = 'ID=' + id +"&DATA=" + data ;
    http.open('POST', 'recuperarRegistros.php', true);
    http.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    // Função a ser chamada no momento em que o servidor responde à requisição
    http.onreadystatechange = function() {
        if(http.readyState == 4 && http.status == 200) {
            desenharGrafico(JSON.parse(this.responseText));
        }
    }
    // Envia a requisição
    http.send(parametros);
}

/*
    Desenha o Gráfico que mostra a evolução dos valores medidos em um uma determinada data
*/
function desenharGrafico(dados) {
    // Configura os eixos do gráfico
    var dadosDoGrafico = new google.visualization.DataTable();
    dadosDoGrafico.addColumn('string', 'Hora');
    dadosDoGrafico.addColumn('number', 'Nivel de Ruido');
    // Popula o vetor com os valores do gráfico
    var i;
    for (i = 0; i < dados.length; i++) { 
        dadosDoGrafico.addRow([dados[i].dataEHora.split(" ")[1],parseFloat(dados[i].nivelRuido)]); 
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