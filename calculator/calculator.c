
#include <stdio.h>
#include "calculator.h"

#define CHAR_IS_SIGN(x)     ((x == '+' || x == '-') ? 1:0)
#define CHAR_IS_NUM(x)      ((x >= '0' && x <= '9') ? 1:0)
#define CHAR_IS_POINT(x)    ((x == '.') ? 1:0)
#define CHAR_TO_VALUE(x)    (x - '0')
#define VALUE_TO_CHAR(x)    (x + '0')
//用于存放中间结果开辟的数组大小设置
#define TEMP_LEN            100 

int str_getlen(char* str)
{
    int i = 0;
    while(str[i]!='\0')
    {
        i++;
    }

    return i;
}

void str_copy(char *des,char *src,int tail_start)
{
    int i = 0;

    if(tail_start == 0)
    {
        while(src[i]!= '\0')
        {
            des[i] = src[i];
            i++;
        }
        des[i] = '\0';
    }
    else
    {
        i = str_getlen(src);
        while(i>=0)
        {
            des[i] = src[i];
            i--;
        }
    }
}

void str_move_left(char *str,int i)
{
    str_copy(str,str + i,0);
}
void str_move_right(char *str,int i)
{
    str_copy(str + i,str ,1);
}

void str_insert_char(char *str,char ch,int insert_index)
{
   // printf("str_insert_char:str = %s insert_index= %d->",str,insert_index);
    str_move_right(str+insert_index,1);
    str[insert_index] = ch;
   // printf("str_insert_char:str = %s\n",str);
}

int str_find_char(char *str,char ch)
{
    int i = 0;
    while(str[i]!='\0')
    {
        if(str[i] == ch)
        {
            return i;
        }
        i++;
    }
    return -1;
}
//123.5*
void str_delete_char(char *str,char ch)
{
    int char_index = 0;

    while(char_index>=0)
    {
        char_index = str_find_char(str,ch);
        if(char_index >= 0)
        {
            str_move_left(str+char_index,1);
        }
    
    }
}

//0001239.219384000
void str_num_format(char *result)
{
   
}
int CALU_uint_mul_10(char* uint_num,int x10)
{
    int num_len = str_getlen(uint_num);
    int i  = 0;
    uint_num[num_len + x10] = uint_num[num_len];

    for(i = 0; i< x10;i++)
    {
        uint_num[num_len + i] = '0';
    }
}

int CALU_uint_div_10(char* uint_num,int x10)
{
    int num_len = str_getlen(uint_num);
    int i  = 0;
    
    if(num_len - x10>0)
    {
        uint_num[num_len - x10] = '\0';
    }
    else
    {
        str_copy(uint_num,"0\0",0);
    }
    
}
int CALU_uint_compare(char* uint_num1,char* uint_num2)
{
    int num1_len = 0;
    int num2_len = 0;
    char num1[TEMP_LEN];
    char num2[TEMP_LEN];
    int i;

    str_copy(num1,uint_num1,0);
    str_copy(num2,uint_num2,0);
    str_num_format(num1);
    str_num_format(num1);
    num1_len = str_getlen(num1);
    num2_len = str_getlen(num2);

    if(num1_len==num2_len)
    {
        for(i=0;i<num1_len;i++)
        {
            if(uint_num1[i] > uint_num2[i])
            {
                return 1;
            }
            if(uint_num1[i] < uint_num2[i])
            {
                return -1;
            }
        }
        return 0;
    }
    else
    {
        if(num1_len > num2_len)
        {
            return 1;
        }
        if(num2_len > num1_len)
        {
            return -1;
        }
    }
}
void CALU_uint_add(char *result,char* uint_num1,char* uint_num2)
{
    int num1_index = str_getlen(uint_num1) - 1;
    int num2_index = str_getlen(uint_num2) - 1;
    int max_len = (num1_index > num2_index)?num1_index:num2_index;
    char num1_ch;
    char num2_ch;
    int int_result=0,carry=0;

    result[max_len+1] = '\0';

    for( ; max_len >= 0;max_len--)
    {   
        if(num1_index >= 0)
        {
            num1_ch = uint_num1[num1_index--];
        }
        else
        {
            num1_ch = '0';
        }

        if(num2_index >= 0)
        {
            num2_ch = uint_num2[num2_index--];
        }
        else
        {
            num2_ch = '0';
        }
        int_result = CHAR_TO_VALUE(num1_ch) + CHAR_TO_VALUE(num2_ch) + carry;
        if(int_result >= 10)
        {
            carry = int_result/10;
            int_result = int_result - 10;
        }
        else
        {
            carry = 0;
        }
        result[max_len] = VALUE_TO_CHAR(int_result);
    }
    if(carry > 0)
    {
        str_insert_char(result,VALUE_TO_CHAR(carry),0);
    }
    
}

int CALU_uint_sub(char *result,char* uint_num1,char* uint_num2)
{
    int num1_index = str_getlen(uint_num1) - 1;
    int num2_index = str_getlen(uint_num2) - 1;
    int max_len = (num1_index > num2_index)?num1_index:num2_index;
    char *sub_num1 , *sub_num2;
    int sub_num1_index, sub_num2_index;
    char num1_ch;
    char num2_ch;
    int int_result=0,carry=0,result_sign = 0;
    int i = 0;

    result[max_len+1] = '\0';

    //确定谁是减数，谁是被减
    if(CALU_uint_compare(uint_num1,uint_num2) >= 0)
    {
        sub_num1 = uint_num1;
        sub_num2 = uint_num2;
        sub_num1_index = num1_index;
        sub_num2_index = num2_index;
        result_sign = 1;
    }
    else
    {
        sub_num1 = uint_num2;
        sub_num2 = uint_num1;
        sub_num1_index = num2_index;
        sub_num2_index = num1_index;
        result_sign = -1;
    }

    for( ;max_len >= 0;max_len--)
    {
        if(sub_num1_index >= 0)
        {
            num1_ch = sub_num1[sub_num1_index--];
        }
        else
        {
            num1_ch = '0';
        }

        if(sub_num2_index >= 0)
        {
            num2_ch = sub_num2[sub_num2_index--];
        }
        else
        {
            num2_ch = '0';
        }
        int_result = CHAR_TO_VALUE(num1_ch) - CHAR_TO_VALUE(num2_ch) + carry;

        if(int_result < 0)
        {
            carry = -1;
            int_result = int_result + 10;
        }
        else
        {
            carry = 0;
        }
        result[max_len] = VALUE_TO_CHAR(int_result);
       // str_insert_char(result,VALUE_TO_CHAR(int_result),0);
    }
    str_num_format(result);
    return result_sign;
}

void CALU_uint_mul(char *result,char* uint_num1,char* uint_num2)
{
    int num1_len = str_getlen(uint_num1) - 1,num1_index;
    int num2_len = str_getlen(uint_num2) - 1,num2_index;
    int max_len = (num1_index > num2_index)?num1_index:num2_index;
    int result_index= 0;
    int uint_result = 0;
    int uint_carry = 0;
    int mul_result_left_move = 0;
    char step_result[TEMP_LEN];
    char sum_result[TEMP_LEN];
    str_copy(step_result,"0\0",0);
    str_copy(sum_result,"0\0",0);

    //step_result[result_index+1] = '\0';

    mul_result_left_move = num2_len;
   
    for(num2_index = num2_len;num2_index >=0 ;num2_index--)
    {
        result_index = num1_len + 1;
        uint_carry = 0;
        step_result[result_index + 1] = '\0';
        for(num1_index =num1_len ;num1_index >=0; num1_index--)
        {
            char num1 = uint_num1[num1_index];
            char num2 = uint_num2[num2_index];
          
            uint_result = CHAR_TO_VALUE(num1) * CHAR_TO_VALUE(num2) + uint_carry;
            if(uint_result >= 10)
            {
                uint_carry = uint_result / 10;
                uint_result = uint_result % 10;
            }
            else
            {
                uint_carry = 0;
            }
            step_result[result_index--] = VALUE_TO_CHAR(uint_result);
        }
        step_result[result_index] = VALUE_TO_CHAR(uint_carry);

        CALU_uint_mul_10(step_result,num2_len - num2_index);

        CALU_uint_add(result,sum_result,step_result);
        str_copy(sum_result,result,0);
        str_num_format(result);
    }
}
void CALU_ufloat_mul(char *result,char* ufloat_num1,char* ufloat_num2)
{
    int num1_point_index = str_find_char(ufloat_num1,'.');
    int num2_point_index = str_find_char(ufloat_num2,'.');
    int num1_len = str_getlen(ufloat_num1);
    int num2_len = str_getlen(ufloat_num2);
    str_delete_char(ufloat_num1,'.');
    str_delete_char(ufloat_num2,'.');
     
    CALU_uint_mul(result,ufloat_num1,ufloat_num2);

    if(num1_point_index > 0)
    {
        num1_point_index = num1_len -1- num1_point_index;
    }
    else
    {
        num1_point_index = 0;
    }
    if(num2_point_index > 0 )
    {
        num2_point_index = num2_len -1- num2_point_index;
    }
    else
    {
        num2_point_index = 0;
    }
    
    if((num1_point_index + num2_point_index)>0)
    {
        str_insert_char(result,'.',str_getlen(result) - (num1_point_index + num2_point_index));
    }
    
}
void CALU_uint_div(char *result,char* uint_num1,char* uint_num2)
{
  char num1[TEMP_LEN],num2[TEMP_LEN],t_result[TEMP_LEN],temp[TEMP_LEN],temp2[TEMP_LEN];
  int Num2_left_steps = 0;
  str_copy(num1,uint_num1,0);
  str_copy(num2,uint_num2,0);
  str_copy(t_result,"0\0",0);
  str_copy(temp2,"0\0",0);
  str_copy(temp,"0\0",0);

  if(uint_num2[0] == '0' && uint_num2[1] == '\0')
  {
      return;
  }
 while(1)
 {
    while(CALU_uint_compare(num1,num2) >= 0)
    {
        CALU_uint_mul_10(num2,1);
        Num2_left_steps++;
    }  
    if(Num2_left_steps > 0)
    {
        
        CALU_uint_div_10(num2,1);
        Num2_left_steps--;
       
        do
        {   
            CALU_uint_sub(temp,num1,num2);
            str_copy(num1,temp,0);
            CALU_uint_add(temp,t_result,"1");
            str_copy(t_result,temp,0);
        } while (CALU_uint_compare(num1,num2) >= 0);

        //把t_result的最低位乘以10的Num2_left_steps次方
        str_copy(temp,"0\0",0);
        temp[0] = t_result[str_getlen(t_result) - 1];
        CALU_uint_mul_10(temp,Num2_left_steps);
        t_result[str_getlen(t_result) - 1] = '0';
        CALU_uint_add(temp2,temp,t_result);
        str_copy(t_result,temp2,0);
       
        CALU_uint_div_10(num2,Num2_left_steps);
        Num2_left_steps = 0;
    }
    else
    {
        str_copy(result,t_result,0);
        return ;
    }
 }
  
}
//1.092*
void CALU_ufloat_div(char *result,char* ufloat_num1,char* ufloat_num2)
{
    int num1_point_index = str_find_char(ufloat_num1,'.');
    int num2_point_index = str_find_char(ufloat_num2,'.');
    int num1_len = str_getlen(ufloat_num1);
    int num2_len = str_getlen(ufloat_num2);
    int mulx = 10 - num1_len;
    str_delete_char(ufloat_num1,'.');
    str_delete_char(ufloat_num2,'.');

    CALU_uint_mul_10(ufloat_num1,mulx);    
    printf("ufloat_num1 = %s ufloat_num2 = %s \n",ufloat_num1, ufloat_num2);
     
    CALU_uint_div(result,ufloat_num1,ufloat_num2);
    printf("447 result = %s \n",result);

    if(num1_point_index > 0)
    {
        num1_point_index = num1_len -1- num1_point_index;
    }
    else
    {
        num1_point_index = 0;
    }
    if(num2_point_index > 0 )
    {
        num2_point_index = num2_len -1- num2_point_index;
    }
    else
    {
        num2_point_index = 0;
    }
    
    if((num1_point_index - num2_point_index)>0)
    {
        str_insert_char(result,'.',str_getlen(result) - (num1_point_index + mulx- num2_point_index));
    }
    
}

void main()
{
    char result[100];
    char num1[100] = "90000",num2[100] ="88000";
     
    CALU_uint_sub(result,num1,num2);
    printf("result =%s\n",result );
}