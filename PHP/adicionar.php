<?php
/*  adicionar.php
 *  Autor: Caio Gomes da Silva Zimmerman
 *  Projeto: Desenvolvimento de um sistema de monitoramento de ruído urbano de baixo custo
 *  Descrição: Página php cujo objetivo é adicionar os dados enviados pelos nós sensores na base de dados através do método POST.
 */
	// Inclui a página conectar.php para utilizar a função Conexao()
    include("conectar.php");
    // Conecta à base de dados 
    $link=Conexao();
    // Recupera os parâmetros através do método POST
    $id = $_POST['ID'];
    $nivel_ruido = $_POST['NIVEL_DE_RUIDO'];
    // Insere os parâmetros na base de dados e fecha a conexão
    $query = "INSERT INTO MEDIDAS (ID_DO_SENSOR, DATA_E_HORA, NIVEL_DE_RUIDO ) VALUES (".$id.",CURRENT_TIMESTAMP(),".$nivel_ruido.");"; 
    mysqli_query($link,$query);
    mysqli_close($link);
?>
