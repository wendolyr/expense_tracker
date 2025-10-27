CREATE EXTENSION IF NOT EXISTS "pgcrypto";

CREATE TABLE IF NOT EXISTS users (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    full_name VARCHAR(255)
);

CREATE TABLE IF NOT EXISTS categories (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    name VARCHAR(100) NOT NULL,
    description TEXT
);

CREATE TABLE expenses (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id UUID NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    amount DECIMAL(12, 2) NOT NULL CHECK (amount > 0),
    category_id UUID NOT NULL REFERENCES categories(id) ON DELETE RESTRICT,
    date DATE NOT NULL
);

INSERT INTO categories (id, name, description) VALUES
(gen_random_uuid(), 'Еда', 'Рестораны и кафе, закуски'),
(gen_random_uuid(), 'Такси', 'Оплата поездок в такси по городу'),
(gen_random_uuid(), 'Поездки между городами', 'Такси, автобусы, поезда, самолёты'),
(gen_random_uuid(), 'Продукты и бытовая химия', 'Оплата в продуктовых магазинах и в магазинах бытовой химии'),
(gen_random_uuid(), 'Одежда', 'Одежда, обувь и аксессуары'),
(gen_random_uuid(), 'Здоровье', 'Медицина, лекарства, спорт'),
(gen_random_uuid(), 'Развлечения', 'Хобби, кино, игры'),
(gen_random_uuid(), 'Алкоголь', 'Бары, магазины'),
(gen_random_uuid(), 'Техника', 'Электронная и бытовая техника'),
(gen_random_uuid(), 'Прочее', 'Прочие расходы');
