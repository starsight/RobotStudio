PROGRAM program0
  VAR
    var1 : DINT;
  END_VAR

  var1 := 0 ;
END_PROGRAM


CONFIGURATION config

  RESOURCE resource1 ON PLC
    TASK task0(INTERVAL := T#20ms,PRIORITY := 0);
    PROGRAM instance0 WITH task0 : program0;
  END_RESOURCE
END_CONFIGURATION
