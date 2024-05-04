create table categories (category text, minnutrition real, maxnutrition real);
insert into categories values
('calories',1800,2200),
('protein',91,1e100),
('fat',0,65),
('sodium',0,1779);

create table foods (food text, cost real);
insert into foods values
('hamburger',2.49),
('chicken',2.89),
('hot dog',1.5),
('fries',1.89),
('macaroni',2.09),
('pizza',1.99),
('salad',2.49),
('milk',0.89),
('ice cream',1.59);

create table nutrition (food text, category text, value real);
insert into nutrition values
('ice cream','protein',8),
('macaroni','protein',12),
('fries','sodium',270),
('fries','calories',380),
('hamburger','fat',26),
('macaroni','sodium',930),
('hot dog','sodium',1800),
('chicken','sodium',1190),
('salad','calories',320),
('ice cream','calories',330),
('milk','sodium',125),
('salad','sodium',1230),
('pizza','sodium',820),
('ice cream','fat',10),
('pizza','protein',15),
('pizza','calories',320),
('hamburger','calories',410),
('milk','fat',2.5),
('salad','protein',31),
('milk','protein',8),
('hot dog','protein',20),
('salad','fat',12),
('hot dog','fat',32),
('chicken','fat',10),
('chicken','protein',32),
('fries','protein',4),
('pizza','fat',12),
('milk','calories',100),
('ice cream','sodium',180),
('chicken','calories',420),
('hamburger','sodium',730),
('macaroni','calories',320),
('fries','fat',19),
('hot dog','calories',560),
('macaroni','fat',10),
('hamburger','protein',24);
