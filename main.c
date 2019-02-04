#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <locale.h>
#include <conio.h>


///********************************************************
///***************** DECLARATION DES TYPES ****************
///********************************************************
typedef struct Cord{
	char matricule[7];
	char nom[31];
	char prenom[31];
	char date_de_naissance[11];
	char wilaya_de_naissance[20];
	char sexe[10];
	char groupe_sanguin[4];
	char date_recrutement[11];
	char grade[30];
	char specialite[30];
    char dernier_diplome[30];
    char etabissement_univ[110];
    char date_de_fin[10];
}Cord;

typedef struct Tbloc{
	Cord T[1000];
	int nb;			//nombre d enregistrement
} Tbloc;

typedef struct Tbloc Buffer;

typedef struct Entete{

	int adrDerBloc;	//vux dir de nombre de bloc dans le fichier
// on peut rajouter ici toute information sur le fichier jugée utile
} Entete;

typedef struct TObF{
	FILE *fichier;
	Entete entete;
} TObF;

TObF *fichi;

//typedef struct liste liste;

typedef struct liste{
	struct liste *suivant;
	char matricule[7];
} liste;

typedef liste *tete;

tete tableau[30];
tete tableau_etab[120];


///*********************************************************
///******************** VARIABLE GLOBALE *******************
///*********************************************************

char date[12];
char nomfich 	  [30]="ENSEIGNANT-MESRS.bin";
char fichier_index[30]="fichier_index.txt";
char univ	      [30]="univ.txt";
char index2 	  [30]="index2.bin"; 
char index3 	  [30]="index3.bin";
char index4 	  [30]="index4.txt";
FILE *file_wilaya;
FILE *file_region;
FILE *file_grade ;
FILE *file_univ  ;
FILE *file_diplome;
FILE *file_specialite;
FILE *file_index ;
FILE *file_index2;
FILE *file_index3;
FILE *file_index4;
int reg;
long nb_eng;


///**********************************************************************
///*********************TOUS LES MODULES UTILISER************************
///**********************************************************************
int  ouvrir		(TObF **f,char *chemin,char mode );
void fermer		(TObF *f						 );
void LireDir	(TObF *f,int N_Bloc,Tbloc *buffer);
void Ecriredir	(TObF *f,int N_Bloc,Tbloc *buffer);
int  get_entete	(TObF *f						 );
void aff_entete	(TObF *f, int i, int val		 );
void alloc_bloc	(TObF *f						 );
int  alea		(int a,int b					 );
void ins_nom	(char mot[31]					 );
void ins_ddn	(char dnn[11],int min,int max	 );
void ins_wil	(char wilaya[20]				 );
void ins_gs		(char gs[4]						 );
void ins_grade	(char grade[30]					 );
void ins_diplome(char diplome[30]				 );
void ins_univ	(char univ[30],char matricule[7] );
void chargement_initial(Cord *cor				 );
int  check_place(FILE *f,char cordo[30],char matric[7]);
void spr		(FILE *f , long matricule		 );
int  get_place	(FILE *f ,char cordo[30],long matricule);
void ins_fich	(FILE *f						 );
void ins_wil	(char wilaya[20]				 );
void get_new_enr(Cord *cor						 );
int  calcule(char date_recrutement[11],int date_min,int date_max);
void Save();
void init();
void sup_sp(liste **l);
void aff_int(FILE *f,long matricule,int date_min,int date_max);
void ecrire(int i,long matricule);
long lire();
int sup_ind4(int i,long matricule);
void creation	(); 
void ins_new	();
void mutation	();
void Supprimer	();


/*--------------Fonction d'ouverture d'un fichier--------------*/

int ouvrir(TObF **f,char *chemin,char mode){
	*f = malloc(sizeof(TObF));
	char s[3];
	if 		((mode == 'A') || (mode == 'a')) sprintf(s,"rb+");
	else if ((mode == 'N') || (mode == 'n')) sprintf(s,"wb+");
	else return 0;

	(*f)->fichier = fopen(chemin, s);
	if((*f)->fichier == NULL) return 0;
	if((mode == 'A') || (mode == 'a')){
		fread(&((*f)->entete), sizeof(Entete), 1, (*f)->fichier);
	}else if ((mode == 'N') || (mode == 'n')){
		(*f)->entete.adrDerBloc = 0;
	}
	rewind((*f)->fichier);
	return 1;
}
/*--------------------------------------------------------------*/
/*--------------Fonction de fermeture d'un fichier--------------*/
void fermer(TObF *f){
	//Sauvegarde de l'entête
	rewind(f->fichier);
	fwrite(&(f->entete), sizeof(Entete), 1, f->fichier);
	fclose(f->fichier);
	free(f);
}
/*--------------------------------------------------------------*/
/*-----------------Fonction de lecture d'un bloc----------------*/
void LireDir(TObF *f,int N_Bloc,Buffer *buf){
	
		fseek(f->fichier, sizeof(Entete) + (N_Bloc-1) * sizeof(Tbloc),SEEK_SET);
		fread(buf, 1, sizeof(Tbloc), f->fichier);
		rewind(f->fichier);	
}
/*--------------------------------------------------------------*/
/*-----------------Fonction d'écriture d'un bloc----------------*/
void Ecriredir(TObF *f,int N_Bloc,Buffer *buf){
		fseek(f->fichier, sizeof(Entete) + (N_Bloc-1) * sizeof(Tbloc),SEEK_SET);
		fwrite(buf, 1, sizeof(Tbloc), f->fichier);
		rewind(f->fichier);
}
/*--------------------------------------------------------------*/
int get_entete(TObF *f){

	return (f->entete).adrDerBloc ;
}
/*--------------------------------------------------------------*/
/*-------------Fonction de modification de l'entête-------------*/
void aff_entete(TObF *f, int i, int val){

	if(i == 1) (f->entete).adrDerBloc = val;
}
/*--------------------------------------------------------------*/
/*--------------------L'allocation d'un bloc--------------------*/
void alloc_bloc(TObF *f){
	Tbloc *buff=malloc(sizeof(Tbloc));        // allocation du Buffer
    buff->nb=0;
    aff_entete(f,1,get_entete(f)+1);
    Ecriredir(f,get_entete(f),buff); // ecriture du buffer dans le bloc representatnt la nouvelle queue
}

///*********************************************************
///***************** PARTIE INITIALISATION *****************
///*********************************************************

void gotoxy(int x , int y){
    COORD coord;
    coord.X=x;
    coord.Y=y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}

///cette fonction retourne un nombre aleatoir entre a et b
int alea(int a,int b){
	
	return (rand()%(b-a))+a;
}
///le role de ce module est d initialiser le nom/prenom
void ins_nom(char mot[31]){
	int i,a=alea(4,30);
	mot[0]=alea(65,90);
	for (i = 1; i < a; i++)
	{
		mot[i]=alea(97,122);
	}
	mot[i]='\0';
}
///le role de ce module est d initialiser la date de naissance
void ins_ddn(char dnn[11],int min,int max){
	int annee=alea(min,max);
	int mois=alea(1,12);
	int jour,i=0;
	if (mois==4||mois==6||mois==9||mois==11){
		jour=alea(1,30);
	}else if(mois==2){
		if(annee % 4 ==0 && annee % 100 !=0){
			jour=alea(1,29);
		}else jour=alea(1,28);
	}else jour=alea(1,31);
	if(jour<10){
		if(mois<10){
			sprintf(dnn,"0%d/0%d/%d",jour,mois,annee);
		}else{
			sprintf(dnn,"0%d/%d/%d",jour,mois,annee);
		}
	}else{
		if(mois<10){
			sprintf(dnn,"%d/0%d/%d",jour,mois,annee);
		}else{
			sprintf(dnn,"%d/%d/%d",jour,mois,annee);
		}
	}
}
///le role de ce module est d initialiser la wilaya
void ins_wil(char wilaya[20]){
	int matric=alea(0,48);
	fseek(file_wilaya,21*matric,SEEK_SET);
	fgets(wilaya,20,file_wilaya);
	wilaya=strtok(wilaya,".");
}
///le role de ce modle est d initialiser le groupe sansguin
void ins_gs(char gs[4]){
	int i=alea(1,8);
	if(i==8){
		strcpy(gs,"AB-");
	}else if(i==1){
		strcpy(gs,"O+");
	}else if(i==2){
		strcpy(gs,"A+");
	}else if(i==3){
		strcpy(gs,"B+");
	}else if(i==4){
		strcpy(gs,"O-");
	}else if(i==5){
		strcpy(gs,"A-");
	}else if(i==6){
		strcpy(gs,"AB+");
	}else if(i==7){
		strcpy(gs,"B-");
	}
}
///le role de modeile est d initialiser le grade
void ins_grade(char grade[30]){
	int matric=alea(0,6);
	fseek(file_grade,32*matric,SEEK_SET);
	fgets(grade,32,file_grade);
	grade=strtok(grade,".");
}
// le role de ce module  est de initialiser la spesialite
void ins_spe(char specialite[30],char matricule[7]){
	int i=alea(0,16);liste *before;
	fseek(file_specialite,25*i,SEEK_SET);
	fgets(specialite,24,file_specialite);
	specialite=strtok(specialite,".");
	before = malloc(sizeof(liste));
	strcpy(before->matricule,matricule);
	before->suivant = tableau[i];
	tableau[i]=before;
}
///le role de ce module est de donne le denier diplome obtenue
void ins_diplome(char diplome[30]){
	int matric=alea(0,5);
	fseek(file_diplome,23*matric,SEEK_SET);
	fgets(diplome,23,file_diplome);
	diplome=strtok(diplome,".");
}
///le role de ce module est de donne l univesite
void ins_univ(char univ[110],char matricule[7]){
	int i=alea(0,120);liste *before;
	fseek(file_univ,110*i,SEEK_SET);
	fgets(univ,109,file_univ);
	univ=strtok(univ,".");
	before = malloc(sizeof(liste));
	strcpy(before->matricule,matricule);
	before->suivant = tableau_etab[i];
	tableau_etab[i]=before;
	if(i<26)		ecrire(1,atol(matricule));
	else if(i<54)	ecrire(2,atol(matricule));
	else 			ecrire(3,atol(matricule));
}
//module prinsipale pour l inisialisation
void chargement_initial(Cord *cor){
	sprintf		(cor->matricule,"%d%d",alea(111,999),alea(111,999));
	ins_nom		(cor->nom);
	ins_nom		(cor->prenom);
	ins_ddn		(cor->date_de_naissance,1953,1992);
	ins_ddn		(cor->date_recrutement,1979,2018);
	ins_wil		(cor->wilaya_de_naissance);
	if(alea(1,2)==1){sprintf(cor->sexe,"masculin");}else sprintf(cor->sexe,"féminin");
	ins_gs		(cor->groupe_sanguin);
	ins_grade	(cor->grade);
	ins_diplome	(cor->dernier_diplome);
}
//le role de ce module est fichier index & epuration
int check_place(FILE *f,char cordo[30],char matric[7]){
	fseek(f,30*(atol(matric)-111111),SEEK_SET);
	fseek(f,6,SEEK_CUR);
	if(fgetc(f)==' '){
		fseek(f,(-1),SEEK_CUR);
		fprintf(f,"%s",cordo );
		rewind(f);
		return 1;
	}else{
		rewind(f);
		return 0;
	}
}
//sup du fichier index
void spr(FILE *f , long matricule){
	fseek(f,6+30*(matricule-111111),SEEK_SET);
	fprintf(f, "                       " );
	sup_ind4(reg,matricule);
}
//sup du fichier principale
int sup_phisiq(Cord *cor, char cordo[31]){
	Buffer *buf=malloc(sizeof(Buffer));
	int nbBloc=atol(strtok(cordo,"/")),pos=atol(strtok(NULL," "));
	LireDir(fichi,get_entete(fichi),buf);
	if(buf->nb==1)
	{
		*cor=buf->T[0];
		aff_entete(fichi,1,get_entete(fichi)-1);
		if(nbBloc==get_entete(fichi)+1)
		{
			return 2;
		}else{
			return 1;//
		}
	}else{
		*cor=buf->T[buf->nb];
		if(nbBloc==get_entete(fichi))
		{
			return 0;
		}else{
			buf->nb--;
			Ecriredir(fichi,get_entete(fichi),buf);
			return 1;//
		}
	}
}
//recuperer le bloc et l ordre de l enregistrement
int get_place(FILE *f ,char cordo[30],long matricule){ 
	fseek(f,6+30*(matricule-111111),SEEK_SET);
	if(fgetc(f)==' '){
		rewind(f);
		return 0;  //le matricule n'existe pas.
	}else{
		
		fgets(cordo,15,f);rewind(f);
		return 1;  //le matricule existe et ses cordonnes sont dans cordo
	}
}
//initialiser index
void ins_fich(FILE *f){
	long i;
	for(i=111111;i<=999999;i++){
		fprintf(f, "%d                      \n", i);
	}
	for (i = 0; i < 3; i++)
	{
		tableau[i]=NULL;
	}
	for (i = 0; i < 3; i++)
	{
		tableau_etab[i]=NULL;
	}
}

void ins_fich2(FILE *f){
	long i;
	fprintf(f,"0     0     0     \n");
	for(i=1;i<=300000;i++){
	fprintf(f,"                  \n");
	}
}
// afficher un enrigistrement 
void afficher(FILE *f,long matricule){
	Buffer *buf = malloc(sizeof(Buffer));
	Cord cor;
	//FILE *f= fopen(fichier_index,"r");
	char cordo[30];int nbboc,pos;
	
	if(get_place(f,cordo,matricule)==1){
		ouvrir(&fichi,nomfich,'A');
		nbboc=atoi(strtok(cordo,"/"));
		pos=atoi(strtok(NULL," "));
		LireDir(fichi,nbboc,buf);
		cor=buf->T[pos];
		printf("Matricule : \t\t%s\nNom : \t\t%s\nPrenom : \t\t%s\nDate de naissance : \t%s\nWilaya de naissance : \t%s\nSexe : \t\t%s\nGroupe sanguin : \t\t%s\nDate de recrutement : \t\t%s\nGrade : \t\t%s\nSpecialite : \t\t%s\nDernier diplome : \t\t%s\nUnivesite : \t\t%s\n\n", 
			cor.matricule, cor.nom,cor.prenom,cor.date_de_naissance,cor.wilaya_de_naissance
			,cor.sexe,cor.groupe_sanguin,cor.date_recrutement,cor.grade, cor.specialite,cor.dernier_diplome,cor.etabissement_univ );
		fermer(fichi);
	}else{
		printf("\nle matricule n existe pas\n");
	}
}
void afficher1(FILE *f,long matricule){
	Buffer *buf = malloc(sizeof(Buffer));
	Cord cor;
	//FILE *f= fopen(fichier_index,"r");
	char cordo[30];int nbboc,pos;
	
	if(get_place(f,cordo,matricule)==1){
		ouvrir(&fichi,nomfich,'A');
		nbboc=atoi(strtok(cordo,"/"));
		pos=atoi(strtok(NULL," "));
		LireDir(fichi,nbboc,buf);
		cor=buf->T[pos];
		printf("--------------------------------------------------------------------------------------------------------\n");
		printf("matricule : %s\tnom : %s\tdate_recrutement : %s\tspecialite : %s\n\t\t\tetabissement_univ : %s\n\n", cor.matricule, cor.nom
		,cor.date_recrutement, cor.specialite,cor.etabissement_univ );
		printf("--------------------------------------------------------------------------------------------------------\n");
		fermer(fichi);
	}
}
//
void get_new_enr(Cord *cor){
	int annee=1900,mois=100,jour=100;
	printf("\nNom : \n");
	gotoxy(6,2);
	scanf("%s",&cor->nom);
	//***************************************************************************************
	printf("\nPrenom : \n");
	gotoxy(9,4);
	scanf("%s",&cor->prenom);
	//***************************************************************************************
zz:	gotoxy(0,5 );
	printf("\nSexe : 1-masculin   2-féminin ");
	scanf("%d",&jour);
	if(jour !=1 && jour !=2 ){
		goto zz;
	}
	if(jour==1){sprintf(cor->sexe,"masculin");}else sprintf(cor->sexe,"féminin");
	//***************************************************************************************
	gotoxy(0,7);
	printf("\nDate de naissance :   /  /      \n");
mh:	gotoxy(20,8);
	printf("  /  /     ");
	gotoxy(20,8);
	scanf("%d",&jour);
	gotoxy(23,8);
	scanf("%d",&mois);
	gotoxy(26,8);
	scanf("%d",&annee);
	
	
	if ((annee<1953 || annee >1995) || (mois<1 || mois>12)){
		goto mh;
		
	}
	if (mois==4||mois==6||mois==9||mois==11){
		if(jour<1 || jour>30){
			goto mh;
		}
	}else if(mois==2){
		if(annee % 4 ==0 && annee % 100 !=0){
			if(jour<1 || jour>29){
				goto mh;
			}
		}else{
			if(jour<1 || jour>28){
				goto mh;
			}
		}
	}else{
		if(jour<1 || jour>31){
			goto mh;
		}
	}

	if(jour<10){
		if(mois<10){
			sprintf(cor->date_de_naissance,"0%d/0%d/%d",jour,mois,annee);
		}else{
			sprintf(cor->date_de_naissance,"0%d/%d/%d",jour,mois,annee);
		}
	}else{
		if(mois<10){
			sprintf(cor->date_de_naissance,"%d/0%d/%d",jour,mois,annee);
		}else{
			sprintf(cor->date_de_naissance,"%d/%d/%d",jour,mois,annee);
		}
	}
	jour=99;
//***************************************************************************************
	printf("\nWilaya de naissance : \n");
	system("wilaya11.txt");
wi:	gotoxy(22,10);
	printf("    ");
	gotoxy(22,10);
	scanf("%d",&jour);
	if(jour<1 || jour>48){
		goto wi;
	}
	jour--;
	FILE *f=fopen("wilaya.txt","r");
	fseek(f,21*jour,SEEK_SET);
	char c=fgetc(f);
	mois=0;
	while(c!='.' && c!=EOF){
		cor->wilaya_de_naissance[mois]=c;
		c=fgetc(f);
		mois++;
	}
	cor->wilaya_de_naissance[mois]='\0';
	fclose(f);
	gotoxy(22,10);
	printf("%s\n", cor->wilaya_de_naissance);
//***************************************************************************************
	printf("\nGroupe sanguin : ");
	printf("    1.O+  2.A+  3.B+  4.O-  5.A-  6.AB+  7.B-  8.AB-\n");
	gotoxy(69,12);
	scanf("%d",&jour);
	if(jour==8){
		strcpy(cor->groupe_sanguin,"AB-");
	}else if(jour==1){
		strcpy(cor->groupe_sanguin,"O+");
	}else if(jour==2){
		strcpy(cor->groupe_sanguin,"A+");
	}else if(jour==3){
		strcpy(cor->groupe_sanguin,"B+");
	}else if(jour==4){
		strcpy(cor->groupe_sanguin,"O-");
	}else if(jour==5){
		strcpy(cor->groupe_sanguin,"A-");
	}else if(jour==6){
		strcpy(cor->groupe_sanguin,"AB+");
	}else if(jour==7){
		strcpy(cor->groupe_sanguin,"B-");
	}
	gotoxy(17,12);
	printf("                                                       ");
	gotoxy(17,12);
	printf("%s\n", cor->groupe_sanguin);
//***************************************************************************************
	printf("\nDate de date_recrutement :     /  /     \n");
dr:	gotoxy(29,14);
	printf("  /  /     ");
	gotoxy(29,14);
	scanf("%d",&jour);
	gotoxy(32,14);
	scanf("%d",&mois);
	gotoxy(35,14);
	scanf("%d",&annee);
	if((annee<1979 || annee >2018) || (mois<1 || mois>12)){
		goto dr;
	}
	if (mois==4||mois==6||mois==9||mois==11){
		if(jour<1 || jour>30){
			goto dr;
		}
	}else if(mois==2){
		if(annee % 4 ==0 && annee % 100 !=0){
			while(jour<1 || jour>29){
				goto dr;
			}
		}else{
			while(jour<1 || jour>28){
				goto dr;
			}
		}
	}else{
		while(jour<1 || jour>31){
			goto dr;
		}
	}
	if(jour<10){
		if(mois<10){
			sprintf(cor->date_recrutement,"0%d/0%d/%d",jour,mois,annee);
		}else{
			sprintf(cor->date_recrutement,"0%d/%d/%d",jour,mois,annee);
		}
	}else{
		if(mois<10){
			sprintf(cor->date_recrutement,"%d/0%d/%d",jour,mois,annee);
		}else{
			sprintf(cor->date_recrutement,"%d/%d/%d",jour,mois,annee);
		}
	}
	//***************************************************************************************
	printf("\nGrade : \n");
	system("gr.txt");
aa:	gotoxy(8,16);
	printf("    ");
	gotoxy(8,16);
	scanf("%d",&mois);
	if( mois<0 || mois >5 ){
		goto aa;
	}
	jour=1;
	f=fopen("grade.txt","r");
    fseek(f,32*mois,SEEK_SET);
	c=fgetc(f);
	jour=0;
	while(c!='.' && c!=EOF){
		cor->grade[jour]=c;
		c=fgetc(f);
		jour++;
	}
	cor->grade[jour]='\0';
	fclose(f);
	gotoxy(8,16);
	printf("%s\n",cor->grade );
	//***************************************************************************************
	printf("\nSpecialite : \n");
	system("sp.txt");
bb:	gotoxy(13,18);
	printf("    ");
	gotoxy(13,18);
	scanf("%d",&mois);
	if( mois<0 || mois >16 ){
		goto bb;
	}
	liste *before;
	f=fopen("specialite.txt","r");
	fseek(f,25*mois,SEEK_SET);
	fgets(cor->specialite,24,f);
	//cor->specialite[0]=strtok(cor->specialite,".");
	before = malloc(sizeof(liste));
	strcpy(before->matricule,cor->matricule);
	before->suivant = tableau[mois];
	tableau[mois]   = before;
	gotoxy(13,18);
	printf("%s\n", cor->specialite );
	fclose(f);

	//***************************************************************************************

	printf("\nUnivesite : ");
	system("univesite.txt");
	scanf("%d",&mois);
	f=fopen("univ.txt","r");
    fseek(f,110*mois,SEEK_SET);
	c=fgetc(f);
	jour=0;
	while(c!='.' && c!=EOF){
		cor->etabissement_univ[jour]=c;
		c=fgetc(f);
		jour++;
	}
	cor->etabissement_univ[jour]='\0';
	fclose(f);
	gotoxy(12,20);
	printf("%s\n",cor->etabissement_univ );
	before = malloc(sizeof(liste));
	strcpy(before->matricule,cor->matricule);
	before->suivant = tableau_etab[mois];
	tableau_etab[mois]=before;

	//***************************************************************************************
	gotoxy(0,21);
	printf("\nDiplome : \n");
	system("dip.txt");
	gotoxy(10,22);
    scanf("%d",&mois);
	f=fopen("diplome.txt","r");
    fseek(f,23*mois,SEEK_SET);
	c=fgetc(f);
	jour=0;
	while(c!='.' && c!=EOF){
		cor->dernier_diplome[jour]=c;
		c=fgetc(f);
		jour++;
	}
	cor->dernier_diplome[jour]='\0';
	fclose(f);
	gotoxy(10,22);
	printf("%s\n", cor->dernier_diplome);
	printf("\n\n");
}
//ce module calcule l enciente
int calcule(char date_recrutement[11],int date_min,int date_max){
	int year;
	strtok(date_recrutement,"/");
	strtok(NULL,"/");
	year=atol(strtok(NULL,"/")); 
	if( year <= date_max && year >= date_min )return 1;
	else return 0;	
} 
//
/*
void affi(){
	liste *last;
	last	=	tableau[1];
	FILE *f =fopen(fichier_index,"r");
	while(last!=NULL){
		afficher(f,atol(last->matricule));
		last=last->suivant;
	}
	fclose(f);
	printf("it works :)\n");
}*/

void Save(){
	file_index2=fopen(index2,"wb");
	file_index3=fopen(index3,"wb");
	int i;liste *last;
	for(i=0;i<2;i++){
		last = tableau[i];
		while(last!=NULL){
			fwrite(last, sizeof(liste),1,file_index2);
			last = last->suivant; 
		}
	}
	for(i=0;i<120;i++){
		last = tableau_etab[i];
		while(last!=NULL){
			fwrite(last, sizeof(liste),1,file_index2);
			last = last->suivant; 
		}
	}
	fclose(file_index2);
	fclose(file_index3);
}

void init(){
	file_index2=fopen(index2,"rb");
	file_index3=fopen(index3,"rb");
	int i;liste *before;
	for(i=0;i<2;i++){
		before = malloc(sizeof(liste));
		fread(before, sizeof(liste),1,file_index2);
		before->suivant=NULL;
		tableau[i]=before;
		before=malloc(sizeof(liste));
		fread(before, sizeof(liste),1,file_index2);
		while(before->suivant!=NULL){
			before->suivant = tableau[i];
			tableau[i]=before;
			before=malloc(sizeof(liste));
			fread(before, sizeof(liste),1,file_index2);
		}
	}
	for(i=0;i<120;i++){
		before = malloc(sizeof(liste));
		fread(before, sizeof(liste),1,file_index2);
		before->suivant=NULL;
		tableau_etab[i]=before;
		before=malloc(sizeof(liste));
		fread(before, sizeof(liste),1,file_index2);
		while(before->suivant!=NULL){
			before->suivant = tableau_etab[i];
			tableau_etab[i]=before;
			before=malloc(sizeof(liste));
			fread(before, sizeof(liste),1,file_index2);
		}
	}
	fclose(file_index2);
	fclose(file_index3);
}

void sup_sp(liste **l){
	if ((*l)!=NULL){
		if(&(*l)->suivant!=NULL){
			sup_sp(&(*l)->suivant);
		}
		Supprimer(atol((*l)->matricule));
		(*l)->suivant=NULL;
		free(*l);
	}
}

void aff_int(FILE *f,long matricule,int date_min,int date_max){
	Buffer *buf = malloc(sizeof(Buffer));
	Cord cor;
	char cordo[30];int nbboc,pos; 
	if(get_place(f,cordo,matricule)==1){
		
		nbboc=atoi(strtok(cordo,"/"));
		pos=atoi(strtok(NULL," "));
		LireDir(fichi,nbboc,buf);
		cor=buf->T[pos];
		cordo[0]='\0';
		strcpy(cordo,cor.date_recrutement);
		if (calcule(cordo,date_min,date_max) == 1)
		{
			//printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n", cor.matricule, cor.nom,cor.prenom,cor.date_de_naissance,cor.wilaya_de_naissance
			//		,cor.sexe,cor.groupe_sanguin,cor.date_recrutement,cor.grade, cor.specialite,cor.dernier_diplome,cor.etabissement_univ );
			printf("--------------------------------------------------------------------------------------------------------\n");
		printf("matricule : %s\tnom : %s\tdate_recrutement : %s\tspecialite : %s\n\t\t\tetabissement_univ : %s\n\n", cor.matricule, cor.nom
		,cor.date_recrutement, cor.specialite,cor.etabissement_univ );
		printf("--------------------------------------------------------------------------------------------------------\n");
		}
		
	}
}

void ecrire(int i,long matricule){
	char place[10];
	fseek(file_index4,6*(i-1),SEEK_SET);
	fgets(place,7,file_index4);
	fseek(file_index4,-6,SEEK_CUR);
	fprintf(file_index4,"      " );
	fseek(file_index4,-6,SEEK_CUR);
	fprintf(file_index4,"%d",atol(place)+1);
	fseek(file_index4,6*(i-1),SEEK_SET);
	fseek(file_index4,20*(atol(place)+1),SEEK_CUR);
	fprintf(file_index4,"%d",matricule );
}

long lire(){//sup est un bool donc si sup=1 a chaque lecrure il va suprimier
	char matric[10];
	fgets(matric,7,file_index4);
	fseek(file_index4,14,SEEK_CUR);	
	return atol(matric);
}

int sup_ind4(int i,long matricule){
	char matric[10],last[10],derMat[7];
	fseek(file_index4,6*(i-1),SEEK_SET);
	fgets(last,7,file_index4);
	fseek(file_index4,20*(atol(last))-6,SEEK_CUR);
	fgets(derMat,7,file_index4);
	rewind(file_index4);
	fseek(file_index4,6*(i-1)+20,SEEK_SET);
	fgets(matric,7,file_index4);
	while( atol(matric)!=matricule && atol(matric)!=0 ){
		fseek(file_index4,14,SEEK_CUR);
		fgets(matric,7,file_index4);
	}
	if( atol(matric) == matricule ){
		fseek(file_index4,-6,SEEK_CUR);
		fprintf(file_index4,"%s",derMat );
		fseek(file_index4,6*(i-1)+20*(atol(last)),SEEK_SET);
		fprintf(file_index4,"      " );
		fseek(file_index4,6*(i-1),SEEK_SET);
		fprintf(file_index4,"%d",(atol(last))-1 );
		return 1;
	}else return 0;
	
}

///*********************************************************
///***************** FONCTIONALITE DEMANDER ****************
///*********************************************************
// module prinsipale pour le remplissage du fichier
void creation(){
	long cpt=0,i,N,var;									     //N enregistrements  // cpt resultat epuration
	Buffer *buf=malloc(sizeof(Buffer));
	Cord cor;
	char cordo[30];
	ouvrir(&fichi,nomfich          ,'N');
	FILE *f		=	fopen(fichier_index	,"w+");
	file_index4	=	fopen(index4	 	,"w+");
	ins_fich(f);
	ins_fich2(file_index4);
	file_wilaya =	fopen("wilaya.txt"  ,"r");
	file_grade  =	fopen("grade.txt"   ,"r");
	file_univ   =	fopen("univ.txt"    ,"r");
	file_diplome=	fopen("diplome.txt" ,"r");
	file_specialite=fopen("specialite.txt","r");
	printf("Combien d enregistrements voulez vous ajouter : N = ");
	scanf("%d",&N);
	printf("ATENTION: cette operation peux prendre des minutes...\n");
	alloc_bloc(fichi);var=1;
	LireDir(fichi,get_entete(fichi),buf);
	for(i=0;i<N;i++){
		chargement_initial(&cor);
		sprintf(cordo,"/%d/%d\0",var,buf->nb );
		if(check_place(f,cordo,cor.matricule)==1){
			ins_spe		(cor.specialite 	,	cor.matricule);
			ins_univ	(cor.etabissement_univ,	cor.matricule);
			if(buf->nb < 1000 ){
				buf->T[buf->nb]=cor;
				buf->nb++;
			}else{
				Ecriredir(fichi,get_entete(fichi),buf);
				alloc_bloc(fichi);var++;
				LireDir(fichi,get_entete(fichi),buf);
				buf->T[buf->nb]=cor;
				buf->nb++;
			}
		}else{
			cpt++;
		}
	}
	printf("le reuultat de l epuration est : %d\n",cpt);
	Ecriredir(fichi,get_entete(fichi),buf);
	fermer(fichi			);
	fclose(f				);
	fclose(file_index4		);
	fclose(file_wilaya		);
	fclose(file_grade 		);
	fclose(file_univ		);
	fclose(file_diplome 	);
	fclose(file_specialite	);
}
//module prinsipale pour ineere un new enregistrement 
void ins_new(){
	Cord cor;
	Buffer *buf=malloc(sizeof(Buffer));
	char cordo[30];
	FILE *f=fopen(fichier_index,"r+");
	ouvrir(&fichi,nomfich          ,'A');
	int j,cpt,trouv;

	LireDir(fichi,get_entete(fichi),buf);
	while(1){
		system("cls");
		trouv=0;
		printf("Matricule :");
		scanf("%s",&cor.matricule);
		if(111111<=atol(cor.matricule) && atol(cor.matricule)<=999999){
			sprintf(cordo,"/%d/%d\0",get_entete(fichi),buf->nb );
			cordo[strlen(cordo)]='\0';
			if(check_place(f,cordo,cor.matricule)==1){
					get_new_enr(&cor);
					if(buf->nb<1000){
						buf->T[buf->nb]=cor;
						buf->nb++;
					}else{
						alloc_bloc(fichi);
						aff_entete(fichi,1,get_entete(fichi)+1);
						LireDir(fichi,get_entete(fichi),buf);
						buf->T[buf->nb]=cor;
						buf->nb++;
					}
					Ecriredir(fichi,get_entete(fichi),buf);
					break;
				}else{
					printf("le matricule entre existe deja. \n voulez vous essayer a nouveau?\n    si 'oui' entre '1' ");
					scanf("%d",&trouv);
					if(trouv!=1){break;}
				}
		}else{printf("*********EROR:le matricule  doit etre entre 111111 et 999999\n");}
		system("cls");
	}
	fermer(fichi);
	fclose(f);
}

void mutation(){
	Cord cor;
	Buffer *buf=malloc(sizeof(Buffer));
	long nbBloc,nbp,newuniv,trouv;
	char cordo[30]="",c;
	ouvrir(&fichi,nomfich      ,'A' );
	FILE *f = fopen(fichier_index,"r+");
	while(1){
		printf("Matricule :");
		scanf("%s",&cor.matricule);
		if(111111<=atol(cor.matricule) && atol(cor.matricule)<=999999){
			if(get_place(f,cordo,atol(cor.matricule))==1){
				nbBloc=atol(strtok(cordo,"/"));
				nbp=atol(strtok(NULL," "));
				LireDir(fichi,nbBloc,buf);
				printf("entre le numero de l' univesite : ");
				system("univesite.txt");
				scanf("%d",&newuniv);
				FILE *f=fopen("univ.txt","r");
				fseek(f,110*newuniv,SEEK_SET);//ici je vais changer le 21 par un autre
				c=fgetc(f);
				trouv=0;
				while(c!='.' && c!=EOF){
					cor.etabissement_univ[trouv]=c;
					c=fgetc(f);
					trouv++;
				}
				cor.etabissement_univ[trouv]='\0';
				fclose(f);
				buf->T[nbp].etabissement_univ[0]='\0';
				strcpy(buf->T[nbp].etabissement_univ,cor.etabissement_univ);
				Ecriredir(fichi,nbBloc,buf);
				break;
			}else{
				printf("le matricule entre n existe pas. \n voulez vous essayer a nouveau?\n    si 'oui' entre '1' ");
				scanf("%d",&trouv);
				if(trouv!=1){break;}
			}
		}else{printf("*********EROR:le matricule  doit etre entre 111111 et 999999\n");}
	}
	fclose(f);
	fermer(fichi);
}
//la suppretion est logique pour le mommbent mais je dois ecrse par le denier enregistremnt
void Supprimer(long matricule){
	Buffer *buf1=malloc(sizeof(Buffer));
	int i,nbboc,pos;
	char cor[30];
	
	if(get_place(file_index,cor,matricule)==1){	
		nbboc=atol(strtok(cor,"/"));
		pos=atol(strtok(NULL," "));
		LireDir(fichi,nbboc,buf1);
		reg=atoi( &buf1->T[pos].etabissement_univ[0]);
		spr(file_index,matricule);
	}else{
		printf("le matricule donnez n'existe pas\n");
	}
	
}
///afficher tous les enregistrements relatifs à une spécialité donnée.
void affi(){
	int i;
	printf("entrer le numero ee leza specialite\n");
	scanf("%d",&i);
	liste *last;
	last	=	tableau[i];
	FILE *f =fopen(fichier_index,"r");
	while(last!=NULL){
		afficher(f,atol(last->matricule));
		last=last->suivant; 
	}
	fclose(f);
}
//Supprimer tous les enregistrements relatifs à une spécialité donnée.
void Supprimer_specialite(){
	file_index=fopen(fichier_index,"r+");
	int i ;
	printf("entre le numero corespond a la specialite que vous vouez Supprimer :  \n");
	scanf("%d",&i);
	sup_sp(&tableau[i]);
	tableau[i]=NULL;
	fclose(file_index);
}
//Afficher tous les enseignants exerçant dans un établissement donné ayant une ancienneté entre deux valeurs données
void Requete_intervalle(){
	int i,date_min,date_max;
	printf("entrer le numero de l univ : ");
	system("univesite.txt");
	scanf("%d",&i);
	printf("entrer la bornne inf :");
	scanf("%d",&date_min);
	printf("entrer la bornne sup :");
	scanf("%d",&date_max);
	liste *last;
	last	=	tableau_etab[i];
	FILE *f =fopen(fichier_index,"r");
	ouvrir(&fichi,nomfich,'A');
	while(last!=NULL){
		aff_int(f,atol(last->matricule),date_min,date_max);
		last=last->suivant;
	}
	fermer(fichi);
	fclose(f);
}

//Consulter tous les enseignants exerçant dans une région donnée
void consuter(){
	file_index4= fopen(index4		 ,"r");
	file_index = fopen(fichier_index ,"r");
	int i;
	printf("1-- ouest  2-- east  3--centre \nquelle region voulez vous aficher : ");
	scanf("%d",&i);
	fseek(file_index4,(i-1)*6+20,SEEK_SET);
	long matricule=lire();
	while(matricule!=0){
		afficher1(file_index, matricule);
		matricule=lire();
	}
	fclose(file_index4);
	fclose(file_index);
}

void menu(){
	int i=0;
    printf("*************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("***************************         ***   ***  ******  ***   **   **   **          ***************************");
    printf("***************************         **** ****  **      ****  **   **   **          ***************************");
    printf("***************************         ** *** **  ******  ** ** **   **   **          ***************************");
    printf("***************************         **  *  **  **      **  ****   **   **          ***************************");
    printf("***************************         **     **  ******  **   ***   *******          ***************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");

    printf("\n");

    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("***************************      1. Creation et epuration                          ***************************");
    printf("***************************      2. Insertion                                      ***************************");
    printf("***************************      3. Mutation                                       ***************************");
    printf("***************************      4. Supprimer un enregistrement                    ***************************");
    printf("***************************      5. Supprimer_specialite                           ***************************");
    printf("***************************      6. Requete_intervalle                             ***************************");
    printf("***************************      7. Consuter                                       ***************************");
    printf("***************************      8. afficher un enregistrement                     ***************************");
    printf("***************************      9.                                                ***************************");
    printf("***************************     10. EXIT                                           ***************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
    printf("**************************************************************************************************************");
}

void check_choix(){
    int choix;long matricule;//cord cor;
    while(1){
        system("cls");
        menu();
        while(1){
            printf("                                           ");
            scanf("%d",&choix);
            if(choix<11 && choix>0){break;}
            else{
                printf("EROR... vous devez choisir apartir du menu \n\n\n");
                system("pause");
                system("cls");
                menu();
            }
        }
           if   (choix == 1){
                system("cls");
                creation();
                system("pause");
        }else if(choix == 2){
            system("cls");
            printf("  ATENTION:  cette operation peux prendre des secondes\n");
            ins_new();
            system("pause");
            system("cls");
            printf("votre demende a ete bien effectue\n ");
        }else if(choix == 3){
            system("cls");
            printf("  ATENTION:  cette operation peux prendre des minutes \n");
            mutation();
            system("cls");
            printf("votre demende a ete bien effectue \n\n\n\n\n\n\n\n\n\n\n\n");
            system("pause");
        }else if(choix == 4){
            system("cls");
            printf("  ATENTION:  cette operation peux prendre des minutes \n");
            ouvrir(&fichi,nomfich          ,'A');
            printf("Entre le matricule : ");
	        scanf("%d",&matricule);
	        file_index =fopen(fichier_index,"r+");
			file_index4=fopen(index4,"r+");
            Supprimer(matricule);
			fermer(fichi);
			fclose(file_index);
			fclose(file_index4);
            system("cls");
            printf("votre demende a ete bien effectue \n\n\n\n\n\n\n\n\n\n\n\n");
            system("pause");
        }else if(choix == 5){
            system("cls");
            printf("  ATENTION:  cette operation peux prendre des minutes \n");
            ouvrir(&fichi,nomfich          ,'A');
            Supprimer_specialite();
            fermer(fichi);
            system("cls");
            printf("votre demende a ete bien effectue \n\n\n\n\n\n\n\n\n\n\n");
            system("pause");
        }else if(choix == 6){
            system("cls");
            printf("  ATENTION:  cette operation peux prendre des minutes \n");
            Requete_intervalle();
            printf("votre demende a ete bien effectue \n\n\n\n\n\n\n\n\n\n\n");
            system("pause");
            system("cls");
        }else if(choix == 7){
            system("cls");
            consuter();
            system("pause");
            system("cls");
        }else if(choix == 8){
            system("cls");
            file_index = fopen(fichier_index ,"r");
            ouvrir(&fichi,nomfich          ,'A');
            printf("Entre le matricule : ");
	        scanf("%d",&matricule);
            afficher(file_index,matricule);
            fermer(fichi);
			fclose(file_index);
            system("pause");
            system("cls");
        }/*else if(choix == 9){
            system("cls");
            printf("Entre le matricule : ");
            scanf("%d",&choix);
            if(get_enr(&cor,choix)==1){
            	//printf("\nmatricule : %s\nnom : %s\nprenom : %s\ndate de naissance : %s\nwilaya de naissance : %s\ngroupe sanguin : %s\ngrade : %s\nforce armee :%s\nregion militaire : %s\n",cor.matricule,cor.nom,cor.prenom,cor.date_de_naissance,cor.wilaya_de_naissance,cor.groupe_sanguin,cor.grade,cor.force_armee,cor.regio_militaire);
       			printf("\nmatricule :           %s\n",cor.matricule);
				printf("nom :                 %s\n",cor.nom);
				printf("prenom :              %s\n",cor.prenom);
				printf("date de naissance :   %s\n",cor.date_de_naissance);
				printf("wilaya de naissance : %s\n",cor.wilaya_de_naissance);
				printf("groupe sanguin :      %s\n",cor.groupe_sanguin);
				printf("grade :               %s\n",cor.grade);
				printf("force armee :         %s\n",cor.force_armee);
				printf("region militaire :    %s\n",cor.regio_militaire);
       			//printf("\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",cor.matricule,cor.nom,cor.prenom,cor.date_de_naissance,cor.wilaya_de_naissance,cor.groupe_sanguin,cor.grade,cor.force_armee,cor.regio_militaire);
            }else{
            	printf("le matricule n existe pas.\n");
            }
            system("pause");
            system("cls");
            printf("votre demende a ete bien effectue \n\n\n\n\n\n\n\n\n\n\n");
            
        }*/else if(choix == 10){
        	break;
        }
    }
}


int main(int argc, char const *argv[])
{
	long matricule;
	system ( " mode con cols=110 lines=28 " );
	setlocale(LC_ALL,"");
	srand(time(NULL));
	/*printf("enter la date actual : ");
	scanf("%s",date);*/
	init();
	check_choix();
	Save();
	return 0;
}
