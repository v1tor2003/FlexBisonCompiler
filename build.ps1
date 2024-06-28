$outProgramName = "prog"

$commands = @(
  "bison -d parser.y",
  "flex lexer.l",
  "g++ main.cpp interp.cpp parser.tab.c lex.yy.c -lfl -o $outProgramName"
)

foreach($command in $commands){
  Invoke-Expression $command
  if($LASTEXITCODE -ne 0){
    Write-Error "$command Failed";
    exit $LASTEXITCODE
  }
}

Write-Output "Build successful. Executable at '$(Get-Location)\$outProgramName' has been created."