<!--records.php
  	Autor: Caio Gomes da Silva Zimmerman
  	Projeto: Desenvolvimento de um sistema de monitoramento de ruído urbano de baixo custo
  	Descrição: Página secundária da interface do usuário onde é possível visualizar a variação do nível de ruido medido por um determinado sensor em um determinado dia.
-->
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Sistema de Monitoramento de Ruído Urbano - Dados Históricos</title>
  <link rel="stylesheet" type="text/css" href="CSS/estiloRegistros.css">
  <link rel="stylesheet" href="//code.jquery.com/ui/1.12.1/themes/base/jquery-ui.css">
  <script src="https://code.jquery.com/jquery-1.12.4.js"></script>
  <script src="https://code.jquery.com/ui/1.12.1/jquery-ui.js"></script>
  <script src="Javascript/scriptRegistros.js"></script>
  <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
</head>
<body>
 
	<p>Data: <input type="text" id="calendario"></p>
	<button onclick="desenharGraficoDeRegistros()">Enviar!</button>
	<div id="grafico"></div>
	<!-- Script em php que recupera o id do sensor selecionado na página principal -->
	<?php
		$id = $_POST['ID'];
		echo '<div id="idSensor">' . $id . '</div>'
	?>
 
 
</body>
</html>