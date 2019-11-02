<?php
/*  conectar.php
 *  Autor: Caio Gomes da Silva Zimmerman
 *  Projeto: Desenvolvimento de um sistema de monitoramento de ruído urbano de baixo custo
 *  Descrição: Página php cujo objetivo é realizar uma conexão do servidor com o banco de dados
 */
	function Conexao(){
		// Dados para conexão
		$server="127.0.0.1";
		$usuario="root";
		$senha="";
		$base_de_dados="sistema_de_monitoramento_de_ruido";
	   	// Cria um link com o banco
		$conexao = mysqli_connect($server, $usuario, $senha, $base_de_dados);
		// Exibe mensagem de erro caso ocorra algum, senão retorn o link com a base de dados
		if (mysqli_connect_errno()){
  			echo "Falha ao conectar ao MySQL: " . mysqli_connect_error();
  		}
		return $conexao;
	}
?>
