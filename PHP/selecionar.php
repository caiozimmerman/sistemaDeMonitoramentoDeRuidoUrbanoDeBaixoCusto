<?php
/* selecionar.php
 * Autor: Caio Gomes da Silva Zimmerman
 * Projeto: Desenvolvimento de um sistema de monitoramento de ruído urbano de baixo custo
 * Descrição: Página php cujo objetivo é selecionar a última medida feita por um determinado sensor para atualizar a interface do usuário
 */
  	// Inclui a página conectar.php para utilizar a função Conexao()
 	include("conectar.php");
  	// Conecta à base de dados
 	$link=Conexao();
  	// Recupera o id do sensor através do método POST
 	$id = $_POST['ID'];
  	// Cria a query e a executa
 	$query = "SELECT NIVEL_DE_RUIDO FROM MEDIDAS WHERE ORDEM=(SELECT MAX(ORDEM ) FROM MEDIDAS WHERE ID_DO_SENSOR =".$id.")"; 
  	$resultado = mysqli_query($link,$query);
  	// Exibe o resultado e fecha a conexão
	$fetch = mysqli_fetch_row($resultado);
  	echo $fetch[0];
 	mysqli_close($link);
?>
