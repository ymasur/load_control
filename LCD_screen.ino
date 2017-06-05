/* LCD_Screen
   ----------
   Module contenant des routines d'affichage
*/

/*
  Calcule de l'énergie en supposant:
  - le courant en mA
  - sommé chaque minute
  - tension secteur 230VAC
  retour: résultat en WH
*/

long energy_computed(unsigned long mAM)
{
  return mAM * 230L / 60000L;
}


/* affiche un nombre en justifiant sur 4 digits
*/
void lcd_nb_spc(long n)
{
  if (n < 10)          // un digit?
    slcd.print(" ");   // oui: mettre un espace
  if (n < 100)         // 2 digits?
    slcd.print(" ");   // oui: mettre un espace
  if (n < 1000)        // 3 digits?
    slcd.print(" ");   // oui: mettre un espace  
}

/* 
*/
void lcd_write()
{
  // curseur placé en colonne 0, line 1
  // à noter que la ligne 0 est la supérieure du LCD
  slcd.setCursor(0, 0);
  slcd.print("Timer:");
  // afficher la variable en décimal:
  lcd_nb_spc(mTimeCharge); 
  slcd.print(mTimeCharge, DEC);
  slcd.print("min");

  if (start)
  { slcd.print("Start");}
  else 
  { 
	if (endLoad && i_AC < i_min )
	{ slcd.print("End  "); }
	else
	{ slcd.print("     ");  }
  }
  // positionner le curseur en 2 ème ligne du LCD  
  slcd.setCursor(0, 1);
  // affichage commuté selon B_affiche
  if (B_affiche)
  {
    slcd.print("Courant:");
    lcd_nb_spc(i_AC);
    // afficher la variable en décimal:
    slcd.print(i_AC, DEC);
    slcd.print("mA");
  }
  else
  {
    long e = energy_computed(mAM);
    slcd.print("Energie:");
    lcd_nb_spc(e);  // justifier à droite
    // afficher la variable en décimal:
    slcd.print(e, DEC);
    slcd.print("WH");
  }
  slcd.print(" ");
}

