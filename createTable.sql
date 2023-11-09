create table dictEn(id int AUTO_INCREMENT, word VARCHAR(100), frequency int, primary key(id));
create table indexEn(alphabet CHAR, indexId int, INDEX(alphabet, indexId));

create table dictCn(id int AUTO_INCREMENT, word VARCHAR(150), frequency int, primary key(id));
create table indexCn(word VARCHAR(15), indexId int, INDEX(word, indexId));

create table webPageLib(id int, url VARCHAR(200), title VARCHAR(500), description  TEXT(500000), primary key(id));
create table invertLib(word VARCHAR(100), webPageId int, weight DOUBLE, INDEX(word, webPageId));
