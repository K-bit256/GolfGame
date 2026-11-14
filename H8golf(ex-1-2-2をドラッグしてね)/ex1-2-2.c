#include<3048f.h>
const unsigned char golf[8]={0x04,0x3a,0x08,0x38,0x6f,0x61,0x72,0xa4};

 void sleepy(int sleeptime){
  int i,j;
  for(i=0;i<sleeptime;i++)
    {for(j=0;j<400;j++);}
}



int main(void)
{
	int row;
	unsigned char	k;

	PB.DDR=0xff;		/*ポートPBを全て出力に設定*/
	P4.DDR=0xff;		/*ポートP4を全て出力に設定*/

    k=0x80;				/*kの初期化。最上位ビットを１
									それ以外を０にする*/

	while(1)			/*無限ループの記述例*/
	{
	  for(row=0;row<8;row++){
		PB.DR.BYTE = ~golf[row];
		P4.DR.BYTE = k;
		k>>=1;				/*kを右１ビットシフト（表示は１行下へ）*/
		if(k==0) k=0x80;	/*一番下まで来たら一番上に戻る*/
	  sleepy(2);				/*sleep()の引数を色々変えて試すと良い*/
	  }
	}

  return 0;
}