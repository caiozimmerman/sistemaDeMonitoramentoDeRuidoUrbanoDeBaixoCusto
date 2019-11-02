<?php
/*  recuperarRegistros.php
 *	Autor: Caio Gomes da Silva Zimmerman
 *	Projeto: Desenvolvimento de um sistema de monitoramento de ruído urbano de baixo custo
 *	Descrição: Página php cujo objetivo é selecionar todos os registros de um determinado sensor em um certo dia e envia-los no formato JSON.
 */
	// Inclui a página conectar.php para utilizar a função Conexao
   	include("conectar.php");
   	// Conecta à base de dados
   	$link=Conexao();
   	// Recupera os parâmetros através do método POST
   	$id = $_POST['ID'];
   	$data = $_POST['DATA'];
   	// Cria a query e a executa
   	$query = "SELECT DATA_E_HORA,NIVEL_DE_RUIDO FROM MEDIDAS WHERE ID_DO_SENSOR=".$id." AND DATA_E_HORA LIKE '".$data."%' ORDER BY DATA_E_HORA;";
   	$resultado = mysqli_query($link,$query);
   	// Exibe os resultados na formato JSON e fecha a conexão
	while($linha = mysqli_fetch_array($resultado)){
	    $nivelRuido = $linha['NIVEL_DE_RUIDO'];
	    $dataEHora = $linha['DATA_E_HORA'];
	    $retorno[] = array("nivelRuido" => $nivelRuido,"dataEHora" => $dataEHora);
	}
	echo json_encode($retorno);
	mysqli_close($link);
?>
