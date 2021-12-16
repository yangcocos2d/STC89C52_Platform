/*
C语言科学计算器(控制台字符界面)

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "calculate.h"

#define NUM_MAX_LEN             15  //操作数最大长度 
#define EQUATION_MAX_LEN        100 //表达式最大长度 
  
typedef struct st_bracket_index 
{ 
   int left_bracket_index;
   int right_bracket_index;
}st_bracket_index; 

st_bracket_index search_inlayer_left_bracket(char *equation)
{
  int i = 0; 
  st_bracket_index st_bracket_index_ret = {-1,-1}; 

  while(equation[i]!=0) 
  { 
    if(equation[i] == '(') 
    {
      st_bracket_index_ret.left_bracket_index = i; 
    }
    if(equation[i] == ')') 
    {
      st_bracket_index_ret.right_bracket_index = i; 
      break; 
    }
    i++; 
  }
  return st_bracket_index_ret; 
}

/*
把字符串src的cut_index_left+1到cut_index_right-1的部分复制给des 
*/
void str_cut(char *des,char *src,int cut_index_left,int cut_index_right) 
{ 
    int i=0,j=0;
    for(i = cut_index_left+1;i<cut_index_right;i++)
    { 
      des[j++] = src[i];
    } 
    des[j++] = '\0';
}

void str_remove_char(char *des,char ch_remove) 
{
  int i;
  int new_des_len = 0;
  int des_len = strlen(des);
  char new_des[EQUATION_MAX_LEN];
  for(i=0;i<des_len;i++)
  {
    if(des[i]!=ch_remove)
    {
      new_des[new_des_len++] = des[i];
    }
  }
  new_des[new_des_len++] = '\0';

  for(i=0;i<new_des_len;i++) 
  {
    des[i] = new_des[i];
  }
} 

/*
在index的前面新增一个字符 
*/
void str_add_char(char *des,char ch_add,int index) 
{
  int len_des = strlen(des);
  int i=0;
  for(i=len_des;i>=index;i--)
  {
    des[i+1]=des[i];
  }
  des[index] = ch_add;
}

/* 
把字符串src粘贴到des的cut_index_left+1到cut_index_right-1部分 
is_del = 1;删除处于cut_index_left和cut_index_right的字符。 
*/
#define CHAR_REMOVE     '#'
void str_paste(char *des,char *src,int paste_index_left,int paste_index_right,int is_del) 
{ 
    int i=0,j=0;
    int len_src = strlen(src);
    int len_des = strlen(des);
    int len_des_cut = paste_index_right - paste_index_left - 1;
    int des_move = len_src - len_des_cut; 
    char ch_remove = CHAR_REMOVE;

    if(is_del == 1) 
    {
      des[paste_index_left] = ch_remove; 
      des[paste_index_right] = ch_remove; 
    }

    if(des_move > 0)
    {
      for(i=len_des;i>=paste_index_right;i--) 
      {
        des[i + des_move] = des[i]; 
      }
    }
    if(des_move < 0) 
    {
      for(i=paste_index_right;i<=len_des;i++) 
      {
        des[i + des_move] = des[i]; 
      }
    }

    for(i=0;i<len_src;i++) 
    {
      des[paste_index_left+ 1 + i] = src[i]; 
    }

    if(is_del == 1) 
    {
        str_remove_char( des, ch_remove );  
    }
  
} 

#define OPA_ADD   '+'
#define OPA_SUB   '~'
#define OPA_MUL   '*'
#define OPA_DIV   '/'
#define OPA_ENDRT '('
#define OPA_END   ')'

typedef struct  st_opa_index 
{
  int opa_pre;
  int opa_cur;
  int opa_next;
}st_opa_index;

int char_is_opa(char ch)
{
  if( ch == OPA_ADD
    ||ch == OPA_SUB
    ||ch == OPA_MUL
    ||ch == OPA_DIV
    ||ch == OPA_ENDRT
    ||ch == OPA_END)
    {
      return 1;
    }
    else
    {
      return 0;
    }
}

/*
    计算形如1/2,1+2这样的最简表达式。
    由于存在0.123456789...这样的小数计算困难的问题(小数有效位有限).
    后续改成基于字符串的计算.

*/
void cal_single_equation(char *equation) 
{
    int     len_equ = strlen(equation);
    char    opa = 0;
    char    Num1[NUM_MAX_LEN]; 
    char    Num2[NUM_MAX_LEN]; 
    float   f_Num1=0;
    float   f_Num2=0;
    float   f_result=0;
    int     i = 0,j = 0;
    int     opa_index = 0;

    for(i=0;i<len_equ;i++)
    {
      if(char_is_opa(equation[i]) == 1)
      {
          opa = equation[i];
          opa_index = i;
          break;
      }
    }

    j=0;
    for(i=0;i<opa_index;i++)
    {
      Num1[j++] = equation[i];
    }
    Num1[j++] = '\0';

    j=0;
    for(i=opa_index + 1;i<len_equ;i++)
    {
      Num2[j++] = equation[i];
    }
    Num2[j++] = '\0';

    f_Num1 = atof(Num1);
    f_Num2 = atof(Num2);
    switch(opa)
    {
      case OPA_ADD:f_result = f_Num1 + f_Num2;break;
      case OPA_SUB:f_result = f_Num1 - f_Num2;break;
      case OPA_MUL:f_result = f_Num1 * f_Num2;break;
      case OPA_DIV:f_result = f_Num1 / f_Num2;break;
      default:break;
    }
    sprintf(equation,"%f",f_result);
}

int char_is_have(char *equation,char ch)
{
  int len = strlen(equation); 
  int i;
  
  for(i=0;i<len;i++)
  { 
    if(equation[i] == ch)
    {
      return 1;
    }
  }
  return 0;
}

/* 
从字符串的index_start 到index_end搜索(包括index_start和index_end)，看有没有ch字符。 
允许index_start < index_end，这样就表示反向搜索。 
其中,ch还有一些特定含义。 
比如ch = CHAR_FIND_OPA时，表示搜索属于操作符的字符，而不具体是某个字符。 
*/ 
#define CHAR_FIND_OPA  0
int char_find_instr(char *equation,int index_start,int index_end,char ch) 
{
  int i=0;
  int ret = -1;
  if(index_start <= index_end)
  { 
    for(i=index_start;i<=index_end;i++)
    {
      if(ch == CHAR_FIND_OPA)
      {
          if(char_is_opa(equation[i]))
          { 
            return i;
          }
      }
      else
      {
         if(equation[i] == ch)
        {
          ret = i;
        }
      }
    }
  } 
  else
  {
    for(i=index_start;i>=index_end;i--)
    {
      if(ch == CHAR_FIND_OPA)
      {
          if(char_is_opa(equation[i]))
          {
            return i;
          }
      }
      else
      {
        if(equation[i] == ch) 
        {
          ret = i;
        }
      }
    }
  }
  return ret;
}


//1+2*3/4
void cal_basic_equation(char *equation) 
{
  int i =0;
  st_opa_index opa_capture;
  int len_equation = strlen(equation);
  char singel_eqution[EQUATION_MAX_LEN];
  float signel_result = 0;
  char opa = OPA_MUL;

  while(1)
  {
    while(char_is_have(equation,opa))
    { 
        len_equation = strlen(equation);
        opa_capture.opa_cur = char_find_instr(equation,0,len_equation,opa);
        opa_capture.opa_pre = char_find_instr(equation,opa_capture.opa_cur-1,0,CHAR_FIND_OPA);
        opa_capture.opa_next = char_find_instr(equation,opa_capture.opa_cur+1,len_equation,CHAR_FIND_OPA);

        str_cut(singel_eqution,equation,opa_capture.opa_pre,opa_capture.opa_next);
        cal_single_equation(singel_eqution);
        str_paste(equation,singel_eqution,opa_capture.opa_pre,opa_capture.opa_next,0);
    } 
    if(opa == OPA_MUL) opa = OPA_DIV;
    else if(opa == OPA_DIV) opa = OPA_ADD;
    else if(opa == OPA_ADD) opa = OPA_SUB;
    else if(opa == OPA_SUB) break;
  }
  str_remove_char(equation,OPA_END);
  str_remove_char(equation,OPA_ENDRT);
} 

//(1+2*(2-3))
void cal_equation(char *equation)
{ 
  char result[EQUATION_MAX_LEN];
  int len_result;
  st_bracket_index st_bracket_index_data;

  while(char_find_instr(equation,0,strlen(equation),CHAR_FIND_OPA)!=-1)
  {
    printf("cal_equation:%s\n",equation);

    st_bracket_index_data = search_inlayer_left_bracket(equation);
    str_cut(result,equation,st_bracket_index_data.left_bracket_index,st_bracket_index_data.right_bracket_index);

    str_add_char(result,OPA_END,0);
    str_add_char(result,OPA_ENDRT,strlen(result));
    cal_basic_equation(result);
    str_remove_char(result,OPA_END);
    str_remove_char(result,OPA_ENDRT);

    str_paste(equation,result,st_bracket_index_data.left_bracket_index,st_bracket_index_data.right_bracket_index,1);
  }
}

//int main(void)
//{
//  char equation[EQUATION_MAX_LEN] = "(1.28+2*(~0.98-~3.09)+(4.09+23.04)/6)"; 
//  cal_equation( equation); 
//  printf("equation:%s\n",equation); 
//  getchar();

//  return 0;
//}
